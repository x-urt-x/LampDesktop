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
			UINT color[3];
			ZeroMemory(color, 3 * sizeof(UINT));
			for (auto colorProc : _colorProcesses)
			{
				color[0] += colorProc->_color[0];
				color[1] += colorProc->_color[1];
				color[2] += colorProc->_color[2];
			}
			for (int i = 0; i < _colorProcesses.size(); i++)
			{
				float br = 255.0 / _cfgs[i]->getBr();
				color[0] += _colorProcesses[i]->_color[0] * br;
				color[1] += _colorProcesses[i]->_color[1] * br;
				color[2] += _colorProcesses[i]->_color[2] * br;
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
