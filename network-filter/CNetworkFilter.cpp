#include "pch.h"
#include "CNetworkFilter.h"
#include "format.h"

#include "CNewConnectionRule.h"
#include "CMaxConnectionsRule.h"
#include "CMaxRecvRule.h"
#include "CMaxSendRule.h"

#include <fstream>
#include <sstream>


CNetworkFilter::CNetworkFilter() {

}

VOID CNetworkFilter::Parse(STRING configurationFile) {
	RULE_VECTOR vector;
	std::ifstream fs(configurationFile);
	STRING line;
	while (std::getline(fs, line)) {
		if (line.find('#') == 0) {
			continue;
		}
		STRING_STREAM stream(line);
		STRING filterType;
		stream >> filterType;

		CNetworkFilterRule* rule = nullptr;
		if (filterType == "new-connection") { rule = new CNewConnectionRule(); }
		else if (filterType == "max-connections") { rule = new CMaxConnectionsRule(); }
		else if (filterType == "max-recv-avg") { rule = new CMaxRecvRule(); }
		else if (filterType == "max-send-avg") { rule = new CMaxSendRule(); }
		else { rule = new CNetworkFilterRule(); }

		if (rule->Parse(stream)) {
			vector.push_back(rule);
		}
		else {
			delete rule;
		}
	}
	this->_rules = vector;
}

BOOL CNetworkFilter::Register(CSocket* socket) {
	RULE_VECTOR& vector = this->_rules;
	BOOL filtered = false;
	for (RULE_ITERATOR it = vector.begin(); it != vector.end(); it++) {
		filtered |= (*it)->Filter(socket);
	}
	if (filtered == FALSE) {
		for (RULE_ITERATOR it = vector.begin(); it != vector.end(); it++) {
			(*it)->Register(socket);
		}
		return TRUE;
	}
	return FALSE;
}

VOID CNetworkFilter::Unregister(CSocket* socket) {
	RULE_VECTOR& vector = this->_rules;
	for (RULE_ITERATOR it = vector.begin(); it != vector.end(); it++) {
		(*it)->Unregister(socket);
	}
}

BOOL CNetworkFilter::Filter(CSocket* socket) {
	BOOL result = false;
	RULE_VECTOR& vector = this->_rules;
	for (RULE_ITERATOR it = vector.begin(); it != vector.end(); it++) {
		result |= (*it)->Filter(socket);
	}
	return result;
}