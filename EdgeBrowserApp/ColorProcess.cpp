#include "ColorProcess.h"
#define ABSF(value) ((value)>=0?(value):-(value))

ColorProcess::ColorProcess(int OutputNumber, float max_ratio_dif) 
	: _outputNumber(OutputNumber), _max_ratio_dif(max_ratio_dif), _mipLevel(0), _mipHeight(0), _mipWidth(0) {
	_color = new BYTE[3];
}

ColorProcess::~ColorProcess()
{
	if(cpu_mipmap)
	cpu_mipmap->Release();
	if(srv)
	srv->Release();
	if(tex_for_srv)
	tex_for_srv->Release();
	if(gpuTex)
	gpuTex->Release();

	if (DeskDupl)
		DeskDupl->Release();

	if (D3DDeviceContext)
		D3DDeviceContext->Release();

	if (D3DDevice)
		D3DDevice->Release();

	DeskDupl = nullptr;
	D3DDeviceContext = nullptr;
	D3DDevice = nullptr;
	HaveFrameLock = false;
}

bool ColorProcess::Initialize()
{
	// Get desktop
	HDESK hDesk = OpenInputDesktop(0, FALSE, GENERIC_ALL);
	if (!hDesk)
		return false;

	// Attach desktop to this thread (presumably for cases where this is not the main/UI thread)
	//bool deskAttached = SetThreadDesktop(hDesk) != 0;
	//CloseDesktop(hDesk);
	//hDesk = nullptr;
	//if (!deskAttached)
	//	return false;

	// Initialize DirectX
	HRESULT hr = S_OK;

	// Driver types supported
	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	auto numDriverTypes = ARRAYSIZE(driverTypes);

	// Feature levels supported
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_1 };
	auto numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D_FEATURE_LEVEL featureLevel;

	// Create device
	for (size_t i = 0; i < numDriverTypes; i++) {
		hr = D3D11CreateDevice(nullptr, driverTypes[i], nullptr, D3D11_CREATE_DEVICE_DEBUG, featureLevels, (UINT)numFeatureLevels,
			D3D11_SDK_VERSION, &D3DDevice, &featureLevel, &D3DDeviceContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return false;

	// Initialize the Desktop Duplication system
	//m_OutputNumber = Output;

	// Get DXGI device
	IDXGIDevice* dxgiDevice = nullptr;
	hr = D3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	if (FAILED(hr))
		return false;

	// Get DXGI adapter
	IDXGIAdapter* dxgiAdapter = nullptr;
	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	dxgiDevice->Release();
	dxgiDevice = nullptr;
	if (FAILED(hr)) {
		return false;
	}

	// Get output
	IDXGIOutput* dxgiOutput = nullptr;
	hr = dxgiAdapter->EnumOutputs(_outputNumber, &dxgiOutput);
	dxgiAdapter->Release();
	dxgiAdapter = nullptr;
	if (FAILED(hr)) {
		return false;
	}

	dxgiOutput->GetDesc(&OutputDesc);

	// QI for Output
	IDXGIOutput1* dxgiOutput1 = nullptr;
	hr = dxgiOutput->QueryInterface(__uuidof(dxgiOutput1), (void**)&dxgiOutput1);
	dxgiOutput->Release();
	dxgiOutput = nullptr;
	if (FAILED(hr))
		return false;

	// Create desktop duplication
	hr = dxgiOutput1->DuplicateOutput(D3DDevice, &DeskDupl);
	dxgiOutput1->Release();
	dxgiOutput1 = nullptr;
	if (FAILED(hr)) {
		return false;
	}
	//получаем размер скрина
	DXGI_OUTDUPL_DESC desc_DeskDupl;
	DeskDupl->GetDesc(&desc_DeskDupl);

	UINT Width = desc_DeskDupl.ModeDesc.Width;
	UINT Height = desc_DeskDupl.ModeDesc.Height;

	//считаем целевой уровень mipmap
	_mipLevel = MipMapLvlCalc(Width, Height);
	_mipWidth = max(1, Width >> _mipLevel);
	_mipHeight = max(1, Height >> _mipLevel);

	//настраиваем промежуточную так как из gpuTex нельзя создать ShaderResourceView из за флагов, флаги же определяются из вне при создании с помощью dupl
	D3D11_TEXTURE2D_DESC desc_tex_for_srv;
	desc_tex_for_srv.SampleDesc.Count = 1;
	desc_tex_for_srv.SampleDesc.Quality = 0;
	desc_tex_for_srv.ArraySize = 1;
	desc_tex_for_srv.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc_tex_for_srv.Height = Height;
	desc_tex_for_srv.Width = Width;
	desc_tex_for_srv.CPUAccessFlags = 0;
	desc_tex_for_srv.MipLevels = _mipLevel + 1;
	desc_tex_for_srv.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc_tex_for_srv.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	desc_tex_for_srv.Usage = D3D11_USAGE_DEFAULT;
	//создаем
	hr = D3DDevice->CreateTexture2D(&desc_tex_for_srv, nullptr, &tex_for_srv);
	if (FAILED(hr)) {
		gpuTex->Release();
		return false;
	}

	//участок который будет копироваться. в данном случае вся текстура. полное копирование делать нельзя из за разных mipmap уровней
	srcBox.left = 0;
	srcBox.top = 0;
	srcBox.front = 0;
	srcBox.right = Width;
	srcBox.bottom = Height;
	srcBox.back = 1;

	//настраиваем шейдер для генерации mipmap
	D3D11_SHADER_RESOURCE_VIEW_DESC desc_srv = {};
	desc_srv.Format = desc_tex_for_srv.Format;
	desc_srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc_srv.Texture2D.MipLevels = _mipLevel + 1;
	//создаем
	hr = D3DDevice->CreateShaderResourceView(tex_for_srv, &desc_srv, &srv);
	if (FAILED(hr)) {
		tex_for_srv->Release();
		gpuTex->Release();
		return false;
	}

	//настраиваем новую текстуру, так как к tex_for_srv нет доступа с cpu. такая же как исходная но уже с mipmap уровнями и с доступом с cpu
	D3D11_TEXTURE2D_DESC desc_cpu_mipmap = desc_tex_for_srv;
	desc_cpu_mipmap.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	desc_cpu_mipmap.Usage = D3D11_USAGE_STAGING;
	desc_cpu_mipmap.BindFlags = 0;
	desc_cpu_mipmap.MiscFlags = 0;
	desc_cpu_mipmap.MipLevels = _mipLevel + 1;
	//создаем
	hr = D3DDevice->CreateTexture2D(&desc_cpu_mipmap, nullptr, &cpu_mipmap);
	if (FAILED(hr)) {
		tex_for_srv->Release();
		gpuTex->Release();
		return false;
	}

	return true;
}

bool ColorProcess::GetColor()
{
	if (!DeskDupl)
		return false;

	HRESULT hr;

	if (HaveFrameLock) {
		HaveFrameLock = false;
		hr = DeskDupl->ReleaseFrame();
		// ignore response
	}

	IDXGIResource* deskRes = nullptr;
	DXGI_OUTDUPL_FRAME_INFO frameInfo;
	hr = DeskDupl->AcquireNextFrame(0, &frameInfo, &deskRes);
	if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
		// nothing to see here
		return false;
	}
	if (FAILED(hr))
		return false;

	HaveFrameLock = true;

	//новый кадр получен

	//тексура в которую придет скрин
	hr = deskRes->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&gpuTex); //получаем скрин
	deskRes->Release();
	deskRes = nullptr;
	if (FAILED(hr)) {
		// not expected
		return false;
	}

	// Выполняем копирование с помощью CopySubresourceRegion
	D3DDeviceContext->CopySubresourceRegion(
		tex_for_srv,                                            // Целевая текстура
		D3D11CalcSubresource(0, 0, _mipLevel), // Нулевой MIP-уровень
		0, 0, 0,
		gpuTex,                                                 // Исходная текстура
		0,                                                      // MIP-уровень исходной текстуры
		&srcBox                                                 // Область копирования
	);

	//создание mipmap
	D3DDeviceContext->GenerateMips(srv);
	//так как mipmap уровни одинаковые то можно копировать полностью
	D3DDeviceContext->CopyResource(cpu_mipmap, tex_for_srv);

	//создаем объект через который получим прямой досутп к текстуре
	D3D11_MAPPED_SUBRESOURCE mappedMip;
	//тут указываем нужный mipmap уровень
	hr = D3DDeviceContext->Map(cpu_mipmap, _mipLevel, D3D11_MAP_READ, 0, &mappedMip);
	if (FAILED(hr)) {
		tex_for_srv->Release();
		cpu_mipmap->Release();
		gpuTex->Release();
		return false;
	}

	//указатель непосредственно на пиксели
	BYTE* srcData = static_cast<BYTE*>(mappedMip.pData);

	//считаем вручную средний цвет. алгоритмы mipmap обрезают тексуру при изменении соотношения сторон
	UINT  Len = _mipWidth * _mipHeight;
	UINT  ColorSum[3] = { 0, 0, 0 };

	for (size_t y = 0; y < _mipHeight; y++) {
		for (size_t x = 0; x < _mipWidth; x++) {
			ColorSum[0] += *(srcData + y * mappedMip.RowPitch + x * 4);
			ColorSum[1] += *(srcData + y * mappedMip.RowPitch + x * 4 + 1);
			ColorSum[2] += *(srcData + y * mappedMip.RowPitch + x * 4 + 2);
		}
	}
	_color[0] = ColorSum[2] / Len;
	_color[1] = ColorSum[1] / Len;
	_color[2] = ColorSum[0] / Len;

	D3DDeviceContext->Unmap(cpu_mipmap, _mipLevel);
	return true;
}

inline UINT ColorProcess::MipMapLvlCalc(UINT Width, UINT Height)
{
	float r = Width / float(Height);
	UINT mipLevel = 1;
	//проверяем все уровни и находим максимальный при котором реальное соотношение в пределах _max_ratio_dif от исходного
	for (UINT i = 1; max(Width, Height) > 1; i++)
	{
		Width /= 2;
		Height /= 2;
		float r1 = float(Width) / float(Height);
		if (ABSF(r - r1) <= _max_ratio_dif)
		{
			mipLevel = i;
		}
	}
	return mipLevel;
}