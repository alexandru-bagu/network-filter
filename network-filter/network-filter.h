#pragma once

#include "CSocket.h"
#include "lock.h"
#include <detours.h>
#include <winsock2.h>
#include <cstdio>

LONG DetourAttach();
LONG DetourDetach();
#if _LOG
VOID DetourStartBackground();
VOID DetourStopBackground();
#endif