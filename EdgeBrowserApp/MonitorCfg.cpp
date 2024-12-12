#include "MonitorCfg.h"

MonitorCfg::MonitorCfg() : name(L"undef"), res(L"undef"), isActive(false), br(255), rate(10), id(0), delay(100), contrast(25)
{
}

bool MonitorCfg::operator==(const MonitorCfg& other) const
{
	return name == other.name && res == other.res && isActive == other.isActive && br == other.br;
}

void MonitorCfg::set(MonitorCfg& cfg)
{
	//std::lock_guard<std::mutex> lock(*mtx.get());
	this->br = cfg.br;
	this->isActive = cfg.isActive;
	this->name = cfg.name;
	this->rate = cfg.rate;
	this->res = cfg.res;
}

CString MonitorCfg::getName() const {
	//std::lock_guard<std::mutex> lock(*mtx.get());
	return name;
}

CString MonitorCfg::getRes() const {
	//std::lock_guard<std::mutex> lock(*mtx.get());
	return res;
}

bool MonitorCfg::getIsActive() const {
	//std::lock_guard<std::mutex> lock(*mtx.get());
	return isActive;
}

UINT8 MonitorCfg::getBr() const {
	//std::lock_guard<std::mutex> lock(*mtx.get());
	return br;
}

UINT8 MonitorCfg::getContrast() const
{
	//std::lock_guard<std::mutex> lock(*mtx.get());
	return contrast;
}

UINT16 MonitorCfg::getRate() const {
	//std::lock_guard<std::mutex> lock(*mtx.get());
	return rate;
}

UINT MonitorCfg::getId() const {
	//std::lock_guard<std::mutex> lock(*mtx.get());
	return id;
}

std::chrono::milliseconds MonitorCfg::getDelay() const
{
	//std::lock_guard<std::mutex> lock(*mtx.get());
	return delay;
}

void MonitorCfg::setName(const CString& newName) {
	//std::lock_guard<std::mutex> lock(*mtx.get());
	name = newName;
}

void MonitorCfg::setRes(const CString& newRes) {
	//std::lock_guard<std::mutex> lock(*mtx.get());
	res = newRes;
}

void MonitorCfg::setIsActive(bool newIsActive) {
	//std::lock_guard<std::mutex> lock(*mtx.get());
	isActive = newIsActive;
}

void MonitorCfg::setBr(UINT8 newBr) {
	//std::lock_guard<std::mutex> lock(*mtx.get());
	br = newBr;
}

void MonitorCfg::setContrast(UINT8 newContrast)
{
	//std::lock_guard<std::mutex> lock(*mtx.get());
	contrast = newContrast;
}

void MonitorCfg::setRate(UINT16 newRate) {
	//std::lock_guard<std::mutex> lock(*mtx.get());
	rate = newRate;
	delay = std::chrono::milliseconds(1000) / newRate;
}

void MonitorCfg::setId(UINT newId) {
	//std::lock_guard<std::mutex> lock(*mtx.get());
	id = newId;
}
