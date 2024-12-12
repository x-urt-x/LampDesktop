#include "framework.h"
#include <mutex>
#pragma once
struct MonitorCfg
{
public:
	MonitorCfg();
	bool operator==(const MonitorCfg& other) const;
	void set(MonitorCfg& cfg);

    CString getName() const;
    CString getRes() const;
    bool getIsActive() const;
    UINT8 getBr() const;
    UINT8 getContrast() const;
    UINT16 getRate() const;
    UINT getId() const;
	std::chrono::milliseconds getDelay() const;

    void setName(const CString& newName);
    void setRes(const CString& newRes);
    void setIsActive(bool newIsActive);
    void setBr(UINT8 newBr);
    void setContrast(UINT8 newContrast);
    void setRate(UINT16 newRate);
    void setId(UINT newId);

private:
	CString name;
	CString res;
	bool isActive;
	UINT8 br;
	UINT8 contrast;
	UINT16 rate;
	UINT id;
	std::chrono::milliseconds delay;

	//std::unique_ptr<std::mutex> mtx;
};