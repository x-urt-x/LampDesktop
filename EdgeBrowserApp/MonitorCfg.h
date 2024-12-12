#include "framework.h"
#pragma once
struct MonitorCfg
{

	CString name;
	CString res;
	bool isActive;
	UINT8 br;
	UINT16 rate;

	bool operator==(const MonitorCfg& other) const
	{
		return name == other.name && res == other.res && isActive == other.isActive && br == other.br;
	}

	MonitorCfg() : name(L"undef"), res(L"undef"), isActive(false), br(255), rate(10) {}
};