#include "UDPControl.h"

UDPControl::UDPControl() : _stopFlag(true)
{

}

UDPControl::~UDPControl()
{
	Stop();
}

void UDPControl::Start(std::vector<MonitorCfg>& cfg, CString ip)
{
	_stopFlag = false;

	for (UINT i = 0; i < cfg.size(); i++)
	{
		auto colorProc = new ColorProcess(i, cfg[i].rate);
		colorProc->Initialize();
		_colorProcesses.push_back(colorProc);
	}
	_lastUpdates = new TPoint[_colorProcesses.size()];
	_delays = new TMs[_colorProcesses.size()];
	for (UINT i = 0; i < _colorProcesses.size(); i++)
	{
		_lastUpdates[i] = TNow;
		_delays[i] = TMs(1000 / cfg[i].rate);
	}
	std::thread proc(&UDPControl::Process, this);
	proc.detach();
	return;
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
	if (_lastUpdates)
		delete _lastUpdates;
	if (_delays)
		delete _delays;
}

void UDPControl::Process()
{
	while (!_stopFlag)
	{
		TPoint now = TNow;
		bool update = false;
		for (UINT i = 0; i < _colorProcesses.size(); i++)
		{
			if (now - _lastUpdates[i] > _delays[i] - TMs(2))
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
			UINT color[3];
			ZeroMemory(color, 3 * sizeof(UINT));
			for (auto colorProc : _colorProcesses)
			{
				color[0] += colorProc->_color[0];
				color[1] += colorProc->_color[1];
				color[2] += colorProc->_color[2];
			}
			color[0] /= _colorProcesses.size();
			color[1] /= _colorProcesses.size();
			color[2] /= _colorProcesses.size();
		}
		std::chrono::nanoseconds time;
		time = now - _lastUpdates[0];
		for (UINT i = 0; i < _colorProcesses.size(); i++)
			if (time > now - _lastUpdates[i])
				time = now - _lastUpdates[i];

		std::this_thread::sleep_for(time);
	}
	_stopFlag = false;
}
