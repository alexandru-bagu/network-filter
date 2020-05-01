#include "pch.h"
#include "CNanoStatistics.h"

CNanoStatistics::CNanoStatistics() {
	this->_running = false;
	this->_value = 0;
	this->_since = this->_until;
}

VOID CNanoStatistics::Start()
{
	this->_running = true;
	this->_value = 0;
	this->_since = CLOCK::now();
}

VOID CNanoStatistics::Stop()
{
	this->_running = false;
	this->_until = CLOCK::now();
}

VOID CNanoStatistics::Add(DOUBLE value)
{
	if (this->_running) {
		this->_value += value;
	}
}

DOUBLE CNanoStatistics::Compute()
{
	auto until = this->_until;
	if (this->_running) {
		until = CLOCK::now();
	}
	auto ns_duration = until - this->_since;
	if (ns_duration.count() == 0) return 0;
	return this->_value / ns_duration.count();
}
