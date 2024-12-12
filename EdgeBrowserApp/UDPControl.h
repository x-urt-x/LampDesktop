#pragma once
#include <WS2tcpip.h>
#include <vector>
#include <chrono>
#include "ColorProcess.h"
#include "MonitorCfg.h"

#define TPoint std::chrono::time_point<std::chrono::steady_clock>
#define TNow std::chrono::high_resolution_clock::now()
#define TMs std::chrono::milliseconds

class UDPControl
{
public:
	UDPControl();
	~UDPControl();
	void Start(std::vector<MonitorCfg>& cfg, CString ip);
	void Stop();
private:
	void Process();
	std::vector<MonitorCfg*> _cfgs;
	std::vector<ColorProcess*> _colorProcesses;
	bool _stopFlag;
	TPoint* _lastUpdates = nullptr;
};

