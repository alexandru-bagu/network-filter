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
	BEGIN_LOCK(this->_syncRoot);
	{
		if (!this->_alive) return;

		delete this->_recvStats;
		delete this->_sendStats;
		this->_alive = false;
	}
	END_LOCK(this->_syncRoot);
}

STRING computeEndPoint(sockaddr_storage sa_storage)
{
	switch (sa_storage.ss_family)
	{
	case AF_INET:
	{
		CHAR ipv4[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(((sockaddr_in*)&sa_storage)->sin_addr), ipv4, INET_ADDRSTRLEN);
		INT32 port = ((sockaddr_in*)&sa_storage)->sin_port;
		return STRING(ipv4) + STRING(":") + TO_STRING(htons(port));
		break;
	}
	case AF_INET6:
	{
		CHAR ipv6[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &(((sockaddr_in6*)&sa_storage)->sin6_addr), ipv6, INET6_ADDRSTRLEN);
		INT32 port = ((sockaddr_in6*)&sa_storage)->sin6_port;
		return STRING(ipv6) + STRING(":") + TO_STRING(htons(port));
		break;
	}
	}
	return STRING("::0");
}

VOID CSocket::computeAddress() {
	sockaddr_storage  sa_storage;
	INT32 len = sizeof(sockaddr_storage);
	INT32 result = getsockname(this->_socket, (sockaddr*)&sa_storage, &len);

	if (result == 0) {
		this->_localEndpoint = computeEndPoint(sa_storage);
	}
	else {
		this->_localEndpoint = STRING("::0");
	}

	len = sizeof(sockaddr_storage);
	result = getpeername(this->_socket, (sockaddr*)&sa_storage, &len);
	if (result == 0) {
		this->_remoteEndpoint = computeEndPoint(sa_storage);
	}
	else {
		this->_remoteEndpoint = STRING("::0");
	}
}

VOID CSocket::ProcessSend(INT32 bytes)
{
	BEGIN_LOCK(this->_syncRoot);
	{
		if (!this->_alive) return;

		this->_sendStats->Add(bytes);
		this->_recvStats->Add(0);
	}
	END_LOCK(this->_syncRoot);
}

VOID CSocket::ProcessReceive(INT32 bytes)
{
	BEGIN_LOCK(this->_syncRoot);
	{
		if (!this->_alive) return;

		this->_recvStats->Add(bytes);
		this->_sendStats->Add(0);
	}
	END_LOCK(this->_syncRoot);
}

DOUBLE CSocket::CurrentDownloadSpeed()
{
	BEGIN_LOCK(this->_syncRoot);
	{
		if (!this->_alive) return 0;
		DOUBLE avg = this->_recvStats->ComputeRecent();
		return avg * this->_second.count();
	}
	END_LOCK(this->_syncRoot);
}

DOUBLE CSocket::AverageDownloadSpeed()
{
	BEGIN_LOCK(this->_syncRoot);
	{
		if (!this->_alive) return 0;
		DOUBLE avg = this->_recvStats->Compute();
		return avg * this->_second.count();
	}
	END_LOCK(this->_syncRoot);
}

DOUBLE CSocket::CurrentUploadSpeed()
{
	BEGIN_LOCK(this->_syncRoot);
	{
		if (!this->_alive) return 0;

		DOUBLE avg = this->_sendStats->ComputeRecent();
		return avg * this->_second.count();
	}
	END_LOCK(this->_syncRoot);
}

DOUBLE CSocket::AverageUploadSpeed()
{
	BEGIN_LOCK(this->_syncRoot);
	{
		if (!this->_alive) return 0;
		DOUBLE avg = this->_sendStats->Compute();
		return avg * this->_second.count();
	}
	END_LOCK(this->_syncRoot);
}

STRING CSocket::LocalEndpoint()
{
	return this->_localEndpoint;
}

STRING CSocket::RemoteEndpoint()
{
	return this->_remoteEndpoint;
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
	return (std::chrono::duration_cast<SECONDS>(CLOCK::now() - _monitoredSince)).count() > 1;
}