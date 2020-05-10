#include "pch.h"
#include "CMaxRecvRule.h"

CMaxRecvRule::CMaxRecvRule() {
	this->_amountLimit = 0;
	::CNetworkFilterRule();
}

BOOL CMaxRecvRule::Parse(STRING_STREAM& stream) {
	STRING str;
	stream >> str; this->_source.Parse(str);
	stream >> str; this->_target.Parse(str);
	stream >> this->_amountLimit;
	CHAR quantifier, unit;
	stream >> quantifier >> unit;
	if (unit != 'B') {
		return FALSE;
	}
	if (quantifier == 'K') {
		this->_amountLimit *= 1024;
	}
	else if (quantifier == 'M') {
		this->_amountLimit *= 1024 * 1024;
	}
	else {
		return FALSE;
	}

	return TRUE;
}

BOOL CMaxRecvRule::Filter(CSocket* socket) {
	return socket->AverageDownloadSpeed() > this->_amountLimit;
}