#pragma once

#include "CSocket.h"
#include "lock.h"
#include <detours.h>
#include <winsock2.h>
#include <cstdio>
#include <map>

typedef std::map<SOCKET, CSocket*> SOCKET_MAP;
typedef std::thread THREAD;
typedef SOCKET_MAP::iterator SOCKET_ITERATOR;

LONG DetourAttach();
LONG DetourDetach();
#if _LOG
VOID DetourStartBackground();
VOID DetourStopBackground();
#endif