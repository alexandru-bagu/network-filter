#pragma once
#include "pch.h"
#include "CNetworkFilterTarget.h"
#include "CSocket.h"
#include "CStatistics.h"
#include "lock.h"
#include <vector>
#include <map>
#include <sstream>

typedef std::istringstream STRING_STREAM;

class CNetworkFilterRule {
protected:
	CNetworkFilterTarget _source, _target;
private:
	MUTEX* _syncRoot;
	SOCKET_VECTOR _sockets;
protected:
	MUTEX* SyncRoot();
	SOCKET_VECTOR_ITERATOR Begin();
	SOCKET_VECTOR_ITERATOR End();
public:
	CNetworkFilterRule();
	
	/// Checks if the socket matches the filter rule
	BOOL Match(CSocket*);

	/// Registers socket to the pool if not filtered
	virtual BOOL Register(CSocket*);

	/// Removes socket from pool
	virtual BOOL Unregister(CSocket*);

	/// Parses the configuration line
	virtual BOOL Parse(STRING_STREAM& stream);

	/// Checks whether a socket is filtered
	virtual BOOL Filter(CSocket*);
};