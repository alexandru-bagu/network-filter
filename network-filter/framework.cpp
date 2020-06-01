#include "pch.h"
#include "framework.h"
#include "lock.h"
#include "path.h"

#include "CNetworkFilter.h"
#include "CSocketLog.h"
#if _LOG
#include <thread>
#endif

SOCKET(WINAPI* TrueAccept)(SOCKET s, struct sockaddr FAR* addr, int FAR* addrlen) = accept;
int(WINAPI* TrueConnect)(SOCKET s, const struct sockaddr FAR* name, int namelen) = connect;
int(WINAPI* TrueRecv)(SOCKET s, char FAR* buf, int len, int flags) = recv;
int(WINAPI* TrueSend)(SOCKET s, const char FAR* buf, int len, int flags) = send;
int(WINAPI* TrueCloseSocket)(SOCKET s) = closesocket;

SOCKET DetourAccept(SOCKET s, struct sockaddr FAR* addr, int FAR* addrlen);
int DetourConnect(SOCKET s, const struct sockaddr FAR* name, int namelen);
int DetourRecv(SOCKET s, char FAR* buf, int len, int flags);
int DetourSend(SOCKET s, const char FAR* buf, int len, int flags);
int DetourCloseSocket(SOCKET s);


static SOCKET_MAP _sockets;
static CNetworkFilter _filters;
MUTEX _frameworkSyncRoot;

#if _LOG
BOOL _thread_running = FALSE;
VOID _background_work(VOID*);
THREAD* _background_thread;

VOID _background_work(VOID* state) {
	LOG->Debug(string_format("::_background_work starting"));
	while (_thread_running) {
		LOG->Debug(string_format("::_background_work tick start"));
		std::this_thread::sleep_for(std::chrono::seconds(1));
		//block the network threads as little as possible
		//to do that we clone the _sockets map and release the lock

		SOCKET_MAP clone;
		BEGIN_LOCK(_frameworkSyncRoot);
		{
			clone = SOCKET_MAP(_sockets);
		}
		END_LOCK(_frameworkSyncRoot);

		CSocketLog log;
		log.Open();
		for (SOCKET_MAP_ITERATOR it = clone.begin(); it != clone.end(); it++) {
			log.Append(it->second);
		}
		log.Close();
		LOG->Debug(string_format("::_background_work tick end"));
	}
}
#endif

VOID break_connection(CSocket* socket)
{
	LOG->Debug(string_format("removing socket with remote endpoint: %s", socket->RemoteEndpoint().c_str()));
	closesocket(socket->Identifier());
	_filters.Unregister(socket);
	LOG->Debug(string_format("removed socket with remote endpoint: %s", socket->RemoteEndpoint().c_str()));
}

///Must be called from a synchronized block
SOCKET_MAP_ITERATOR register_socket(SOCKET s) {
	SOCKET_MAP_ITERATOR iter;
	CSocket* socket = new CSocket(s);
	LOG->Debug(string_format("registering new socket with remote endpoint: %s", socket->RemoteEndpoint().c_str()));
	_sockets.insert(std::make_pair(s, socket));
	_filters.Register(socket);
	LOG->Debug(string_format("registered new socket with remote endpoint: %s", socket->RemoteEndpoint().c_str()));
	return _sockets.find(s);
}

SOCKET DetourAccept(SOCKET s, struct sockaddr FAR* addr, int FAR* addrlen)
{
	SOCKET res = TrueAccept(s, addr, addrlen);
	CSocket* socket = new CSocket(res);
	LOG->Debug(string_format("::DetourAccept RemoteEndpoint: %s", socket->RemoteEndpoint().c_str()));
	
	if (_filters.Filter(socket)) {
		break_connection(socket);
		return INVALID_SOCKET;
	}

	BEGIN_LOCK(_frameworkSyncRoot);
	{
		register_socket(res);
	}
	END_LOCK(_frameworkSyncRoot);
	return res;
}

int DetourConnect(SOCKET s, const struct sockaddr FAR* name, int FAR namelen)
{
	int res = TrueConnect(s, name, namelen);
	if (res == 0) {
		CSocket* socket = new CSocket(s);
		LOG->Debug(string_format("::TrueConnect RemoteEndpoint: %s", socket->RemoteEndpoint().c_str()));
		if (_filters.Filter(socket)) {
			break_connection(socket);
			return SOCKET_ERROR;
		}

		BEGIN_LOCK(_frameworkSyncRoot);
		{
			register_socket(s);
		}
		END_LOCK(_frameworkSyncRoot);
	}
	return res;
}

int DetourRecv(SOCKET s, char FAR* buf, int len, int flags)
{
	CSocket socket(s);
	LOG->Debug(string_format("::DetourRecv Start RemoteEndpoint: %s", socket.RemoteEndpoint().c_str()));
	int res = TrueRecv(s, buf, len, flags);

	SOCKET_MAP_ITERATOR iter;
	BEGIN_LOCK(_frameworkSyncRoot);
	{
		iter = _sockets.find(s);

		//in case dll was attached after connections were already active
		if (iter == _sockets.end()) {
			iter = register_socket(s);
		}
	}
	END_LOCK(_frameworkSyncRoot);
	LOG->Debug(string_format("::DetourRecv RemoteEndpoint: %s", iter->second->RemoteEndpoint().c_str()));

	iter->second->ProcessReceive(res);

	if (_filters.Filter(iter->second)) {
		break_connection(iter->second);
		return 0;
	}
	LOG->Debug(string_format("::DetourRecv End RemoteEndpoint: %s", socket.RemoteEndpoint().c_str()));
	return res;
}

int DetourSend(SOCKET s, const char FAR* buf, int len, int flags)
{
	CSocket socket(s);
	LOG->Debug(string_format("::DetourSend Start RemoteEndpoint: %s", socket.RemoteEndpoint().c_str()));
	int res = TrueSend(s, buf, len, flags);

	SOCKET_MAP_ITERATOR iter;
	BEGIN_LOCK(_frameworkSyncRoot);
	{
		iter = _sockets.find(s);

		//in case dll was attached after connections were already active
		if (iter == _sockets.end()) {
			iter = register_socket(s);
		}
	}
	END_LOCK(_frameworkSyncRoot);
	LOG->Debug(string_format("::DetourSend RemoteEndpoint: %s", iter->second->RemoteEndpoint().c_str()));

	iter->second->ProcessSend(res);
	if (_filters.Filter(iter->second)) {
		break_connection(iter->second);
		res = 0;
	}
	LOG->Debug(string_format("::DetourSend End RemoteEndpoint: %s", socket.RemoteEndpoint().c_str()));
	return res;
}

int DetourCloseSocket(SOCKET s)
{
	int res = TrueCloseSocket(s);

	BEGIN_LOCK(_frameworkSyncRoot);
	{
		auto iter = _sockets.find(s);
		//just in case `TrueSend` returns after `closesocket` was called
		//or in case dll was attached after connections were already active
		//or connection was blocked at start
		if (iter != _sockets.end()) {
			LOG->Debug(string_format("::DetourCloseSocket RemoteEndpoint: %s", iter->second->RemoteEndpoint().c_str()));
			delete iter->second;
			_sockets.erase(iter);
		}
	}
	END_LOCK(_frameworkSyncRoot);
	return res;
}

LONG DetourAttach()
{
	_filters.Parse(path::combine(path::app_path(), "network-filter.cfg"));

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)TrueAccept, DetourAccept);
	DetourAttach(&(PVOID&)TrueConnect, DetourConnect);
	DetourAttach(&(PVOID&)TrueRecv, DetourRecv);
	DetourAttach(&(PVOID&)TrueSend, DetourSend);
	DetourAttach(&(PVOID&)TrueCloseSocket, DetourCloseSocket);
	return DetourTransactionCommit();
}

LONG DetourDetach()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)TrueAccept, DetourAccept);
	DetourDetach(&(PVOID&)TrueConnect, DetourConnect);
	DetourDetach(&(PVOID&)TrueRecv, DetourRecv);
	DetourDetach(&(PVOID&)TrueSend, DetourSend);
	DetourDetach(&(PVOID&)TrueCloseSocket, DetourCloseSocket);
	return DetourTransactionCommit();
}

#if _LOG
VOID DetourStartBackground()
{
	if (!_thread_running) {
		_thread_running = true;
		_background_thread = new THREAD(_background_work, nullptr);
	}
}

VOID DetourStopBackground()
{
	_thread_running = false;
}
#endif