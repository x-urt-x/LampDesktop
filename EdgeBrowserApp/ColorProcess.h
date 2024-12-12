#pragma once
#include <d3d11.h>
#include <dxgi1_2.h>
#include <atomic>
#include <condition_variable>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")


class ColorProcess
{
public:
	ColorProcess(int OutputNumber, float max_ratio_dif = 0.05);
	~ColorProcess();

	bool Initialize();
	BYTE* _color;
	bool GetColor();

private:

	int    _outputNumber;
	float _max_ratio_dif;

	UINT _mipLevel;
	UINT  _mipWidth;
	UINT  _mipHeight;

	inline UINT MipMapLvlCalc(UINT Width, UINT Height);

	ID3D11Device* D3DDevice = nullptr;
	ID3D11DeviceContext* D3DDeviceContext = nullptr;
	IDXGIOutputDuplication* DeskDupl = nullptr;
	DXGI_OUTPUT_DESC        OutputDesc;
	bool                    HaveFrameLock = false;


	ID3D11Texture2D* gpuTex = nullptr;
	ID3D11Texture2D* tex_for_srv = nullptr;
	D3D11_BOX srcBox;
	ID3D11ShaderResourceView* srv = nullptr;
	ID3D11Texture2D* cpu_mipmap = nullptr;

};