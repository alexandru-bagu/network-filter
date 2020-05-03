#pragma once
#include "pch.h"
#include "CNetworkFilterTarget.h"
#include "CSocket.h"
#include "lock.h"
#include <vector>

typedef std::vector<CSocket*> SOCKET_VECTOR;
typedef SOCKET_VECTOR::iterator SOCKET_ITERATOR;

//new-connection *:5816 *:* 2/1s
//max-connections *:5816 *:* 10
//max-recv-avg *:5816 *:* 50KB/1s
//max-send-avg *:5816 *:* 50KB/1s

class CNetworkFilterRule {
private:
	CNetworkFilterTarget _source, _target;
	MUTEX _syncRoot;
	SOCKET_VECTOR _sockets;
protected:
	MUTEX& SyncRoot();
	SOCKET_ITERATOR Begin();
	SOCKET_ITERATOR End();
public:
	CNetworkFilterRule();
	BOOL Match(CSocket*);
	virtual BOOL Register(CSocket*);
	virtual BOOL Remove(CSocket*);
	virtual VOID Tick();

	virtual BOOL Parse(STRING line) = 0;
	virtual BOOL Filter(CSocket*) = 0;
};