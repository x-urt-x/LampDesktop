#include "UDPControl.h"

UDPControl::UDPControl() : _stopFlag(true)
{

}

UDPControl::~UDPControl()
{
	Stop();
}

void UDPControl::Start(HWND hWndMain, std::vector<MonitorCfg>& cfg, CString ip)
{
	WSADATA data;
	WORD version = MAKEWORD(2, 2);

	// Start WinSock
	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0)
		return;

	server.sin_family = AF_INET;
	server.sin_port = htons(8888);
	inet_pton(AF_INET, CStringA(ip).GetString(), &server.sin_addr);
	out = socket(AF_INET, SOCK_DGRAM, 0);


	_hWndMain = hWndMain;
	for (int i = 0; i < cfg.size(); i++)
	{
		if (cfg[i].getIsActive())
		{
			_cfgs.push_back(&cfg[i]);
		}
	}

	if (!_cfgs.size())
		return;

	_stopFlag = false;

	for (UINT i = 0; i < _cfgs.size(); i++)
	{
		auto colorProc = new ColorProcess(_cfgs[i]->getId());
		colorProc->Initialize();
		_colorProcesses.push_back(colorProc);
	}
	_lastUpdates = new TPoint[_colorProcesses.size()];
	for (UINT i = 0; i < _colorProcesses.size(); i++)
	{
		_lastUpdates[i] = TNow;
	}
	std::thread proc(&UDPControl::Process, this);
	proc.detach();
}

void UDPControl::Stop()
{
	if (!_stopFlag)
	{
		_stopFlag = true;
		while (_stopFlag) {}
	}
	_stopFlag = true;
	if (_colorProcesses.size() > 0)
	{
		for (auto colorProc : _colorProcesses)
			delete colorProc;
		_colorProcesses.clear();
	}
	_cfgs.clear();
	if (_lastUpdates)
		delete _lastUpdates;

	closesocket(out);
	WSACleanup();
}

void UDPControl::Process()
{
	while (!_stopFlag)
	{
		TPoint now = TNow;
		bool update = false;
		for (UINT i = 0; i < _colorProcesses.size(); i++)
		{
			if (now - _lastUpdates[i] > _cfgs[i]->getDelay() - TMs(2))
			{
				if (_colorProcesses[i]->GetColor())
				{
					_lastUpdates[i] = now;
					update = true;
				}
			}
		}
		if (update)
		{
			UINT32 color[3];
			ZeroMemory(color, 3 * sizeof(UINT));
			for (int i = 0; i < _colorProcesses.size(); i++)
			{
				float br = 255.0 / _cfgs[i]->getBr();
				color[0] += _colorProcesses[i]->_color[0] * br;
				color[1] += _colorProcesses[i]->_color[1] * br;
				color[2] += _colorProcesses[i]->_color[2] * br;
			}
			BYTE colorToSend[3];
			colorToSend[0] = color[0] / _colorProcesses.size();
			colorToSend[1] = color[1] / _colorProcesses.size();
			colorToSend[2] = color[2] / _colorProcesses.size();

			PostMessage(_hWndMain, WM_SET_BUTTON_COLOR, 0, (LPARAM)colorToSend);
			sendto(out, reinterpret_cast<char*>(colorToSend), 3, 0, (sockaddr*)&server, sizeof(server));
		}
		std::chrono::nanoseconds time;
		time = now - _lastUpdates[0];
		for (UINT i = 0; i < _colorProcesses.size(); i++)
			if (time > now - _lastUpdates[i] && now != _lastUpdates[i])
				time = now - _lastUpdates[i];
		std::this_thread::sleep_for(time);
	}
	_stopFlag = false;
}
