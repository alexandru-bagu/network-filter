#include "pch.h"
#include "CNetworkFilterTarget.h"
#include "endpoint.h"


CNetworkFilterTarget::CNetworkFilterTarget() {
	this->_address = "*";
	this->_port = "*";
}

BOOL CNetworkFilterTarget::Parse(STRING endpoint) {
	auto ep = endpoint_parse(endpoint);
	if (ep.first != "") {
		this->_address = ep.first;
		this->_port = ep.second;
		return true;
	}
	return false;
}

BOOL CNetworkFilterTarget::Match(STRING endpoint) {
	if (this->_address == "*" && this->_port == "*") {
		return true;
	}
	auto ep = endpoint_parse(endpoint);
	BOOL result = true;
	if (this->_address != "*") {
		result &= (BOOL)(ep.first == this->_address);
	}
	if (this->_port != "*") {
		result &= (BOOL)(ep.second == this->_port);
	}
	return result;
}