#include "pch.h"
#include "CSocket.h"

static volatile INT64 _csocketId = 0;

CSocket::CSocket(SOCKET s)
{
	auto sampleTimespan = MILLISECONDS(50);

	this->_id = ++_csocketId;
	this->_socket = s;
	this->_alive = true;
	this->_second = SECONDS(1);
	this->_recvStats = new CStatistics(sampleTimespan, 100);
	this->_sendStats = new CStatistics(sampleTimespan, 100);
	this->_monitoredSince = CLOCK::now();
	this->computeAddress();
}

CSocket::~CSocket()
{
	LOCK(this->_syncRoot,
		if (!this->_alive) return;

	delete this->_recvStats;
	delete this->_sendStats;
	this->_alive = false;
	)
}

VOID CSocket::computeAddress() {
	sockaddr_storage  sa_storage;
	INT32 len = sizeof(sockaddr_storage);
	INT32 result = getsockname(this->_socket, (sockaddr*)&sa_storage, &len);

	if (result == 0) {
		switch (sa_storage.ss_family)
		{
		case AF_INET:
			CHAR ipv4[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(((sockaddr_in*)&sa_storage)->sin_addr), ipv4, INET_ADDRSTRLEN);
			this->_addr = STRING(ipv4);
			this->_port = ((sockaddr_in*)&sa_storage)->sin_port;
			break;
		case AF_INET6:
			CHAR ipv6[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, &(((sockaddr_in6*)&sa_storage)->sin6_addr), ipv6, INET6_ADDRSTRLEN);
			this->_addr = STRING(ipv6);
			this->_port = ((sockaddr_in6*)&sa_storage)->sin6_port;
			break;
		}
	}
	else {
		this->_addr = STRING("0.0.0.0");
		this->_port = 0;
	}
}

VOID CSocket::ProcessSend(INT32 bytes)
{
	LOCK(this->_syncRoot,
		if (!this->_alive) return;

	this->_sendStats->Add(bytes);
	this->_recvStats->Add(0);
	)
}

VOID CSocket::ProcessReceive(INT32 bytes)
{
	LOCK(this->_syncRoot,
		if (!this->_alive) return;

	this->_recvStats->Add(bytes);
	this->_sendStats->Add(0);
	);
}

DOUBLE CSocket::CurrentDownloadSpeed()
{
	LOCK(this->_syncRoot,
		if (!this->_alive) return 0;
	DOUBLE avg = this->_recvStats->ComputeRecent();
	return avg * this->_second.count();
	)
}

DOUBLE CSocket::AverageDownloadSpeed()
{
	LOCK(this->_syncRoot,
		if (!this->_alive) return 0;
	DOUBLE avg = this->_recvStats->Compute();
	return avg * this->_second.count();
	)
}

DOUBLE CSocket::CurrentUploadSpeed()
{
	LOCK(this->_syncRoot,
		if (!this->_alive) return 0;

	DOUBLE avg = this->_sendStats->ComputeRecent();
	return avg * this->_second.count();
	)
}

DOUBLE CSocket::AverageUploadSpeed()
{
	LOCK(this->_syncRoot,
		if (!this->_alive) return 0;
	DOUBLE avg = this->_sendStats->Compute();
	return avg * this->_second.count();
	)
}

STRING CSocket::Address()
{
	return this->_addr;
}

INT32 CSocket::Port()
{
	return this->_port;
}

SOCKET CSocket::Socket()
{
	return this->_socket;
}

INT64 CSocket::Identifier()
{
	return this->_id;
}

BOOL CSocket::Reliable()
{
	return (std::chrono::duration_cast<SECONDS>(CLOCK::now() - _monitoredSince)).count() > 4;
}