#include "pch.h"
#include "network-filter.h"
#include "filter-rules.h"
#include "lock.h"
#include "log.h"
#include <thread>
#include <map>

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

typedef std::map<SOCKET, CSocket*> SOCKET_MAP;
typedef SOCKET_MAP::iterator SOCKET_ITERATOR;
typedef std::thread THREAD;

static SOCKET_MAP _sockets;
MUTEX _frameworkSyncRoot;

#if _LOG
BOOL _thread_running;
VOID _background_work(VOID*);
THREAD* _background_thread;

VOID _background_work(VOID* state) {
	while (_thread_running) {
		std::this_thread::sleep_for(std::chrono::seconds(1));

		//block the network threads as little as possible
		//to do that we clone the _sockets map and release the lock
		SOCKET_MAP* clone;
		LOCK(_frameworkSyncRoot,
			clone = new SOCKET_MAP(_sockets);
		);

		log_open();
		for (SOCKET_ITERATOR it = clone->begin(); it != clone->end(); it++) {
			log_stats(it->second);
		}
		log_close();

		delete clone;
	}
}
#endif

VOID break_connection(CSocket* socket)
{
	closesocket(socket->Identifier());
}

///Must be called from a synchronized block
SOCKET_ITERATOR register_socket(SOCKET s) {
	SOCKET_ITERATOR iter;
	CSocket* socket = new CSocket(s);
	_sockets.insert(std::make_pair(s, socket));
	return _sockets.find(s);
}

SOCKET DetourAccept(SOCKET s, struct sockaddr FAR* addr, int FAR* addrlen)
{
	SOCKET res = TrueAccept(s, addr, addrlen);
	CSocket* socket = new CSocket(res);
	if (endpoint_is_blocked(socket)) {
		break_connection(socket);
		return INVALID_SOCKET;
	}

	LOCK(_frameworkSyncRoot,
		register_socket(res);
	);
	return res;
}

int DetourConnect(SOCKET s, const struct sockaddr FAR* name, int FAR namelen)
{
	int res = TrueConnect(s, name, namelen);
	if (res == 0) {
		CSocket* socket = new CSocket(s);
		if (endpoint_is_blocked(socket)) {
			break_connection(socket);
			return SOCKET_ERROR;
		}

		LOCK(_frameworkSyncRoot,
			register_socket(s);
		);
	}
	return res;
}

int DetourRecv(SOCKET s, char FAR* buf, int len, int flags)
{
	int res = TrueRecv(s, buf, len, flags);

	SOCKET_ITERATOR iter;
	LOCK(_frameworkSyncRoot,
		iter = _sockets.find(s);

	//in case dll was attached after connections were already active
	if (iter == _sockets.end()) {
		iter = register_socket(s);
	});

	iter->second->ProcessReceive(res);
	if (socket_check_recv_stats(iter->second)) {
		break_connection(iter->second);
		return 0;
	}
	return res;
}

int DetourSend(SOCKET s, const char FAR* buf, int len, int flags)
{
	int res = TrueSend(s, buf, len, flags);

	SOCKET_ITERATOR iter;
	LOCK(_frameworkSyncRoot,
		iter = _sockets.find(s);

	//in case dll was attached after connections were already active
	if (iter == _sockets.end()) {
		iter = register_socket(s);
	});

	iter->second->ProcessSend(res);
	if (socket_check_send_stats(iter->second)) {
		break_connection(iter->second);
		return 0;
	}
	return res;
}

int DetourCloseSocket(SOCKET s)
{
	int res = TrueCloseSocket(s);

	LOCK(_frameworkSyncRoot,
		auto iter = _sockets.find(s);
	//just in case `TrueSend` returns after `closesocket` was called
	//or in case dll was attached after connections were already active
	//or connection was blocked at start
	if (iter != _sockets.end()) {
		delete iter->second;
		_sockets.erase(iter);
	});
	return res;
}

LONG DetourAttach()
{
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