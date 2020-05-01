#pragma once
#include "CStatistics.h"
#include "lock.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>

typedef std::string STRING;

class CSocket
{
private:
	INT64 _id;
	MUTEX _syncRoot;
	BOOL _alive;
	SOCKET _socket;
	STRING _addr;
	INT32 _port;
	NANOSECONDS _second;
	CStatistics* _recvStats, * _sendStats;
	TIME_POINT _monitoredSince;
	VOID computeAddress();
public:
	CSocket(SOCKET s);
	~CSocket();
	VOID ProcessReceive(INT32 bytes);
	VOID ProcessSend(INT32 bytes);

	/// Returns true if the download/speeds can be taken into account for filtering
	BOOL Reliable();

	/// Returns received bytes per second in Bs
	DOUBLE CurrentDownloadSpeed();
	/// Returns received bytes per second in Bs
	DOUBLE AverageDownloadSpeed();
	/// Returns sent bytes per second in Bs
	DOUBLE CurrentUploadSpeed();
	/// Returns sent bytes per second in Bs
	DOUBLE AverageUploadSpeed();

	STRING Address();
	INT32 Port();
	SOCKET Socket();
	INT64 Identifier();
};