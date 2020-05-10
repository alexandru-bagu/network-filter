#include "pch.h"
#include "CMaxSendRule.h"

CMaxSendRule::CMaxSendRule() {
	this->_amountLimit = 0;
	::CNetworkFilterRule();
}

BOOL CMaxSendRule::Parse(STRING_STREAM& stream) {
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

BOOL CMaxSendRule::Filter(CSocket* socket) {
	return socket->AverageUploadSpeed() > this->_amountLimit;
}