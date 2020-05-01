#pragma once
#include "CSocket.h"
#include "lock.h"

VOID log_close();
VOID log_open();
VOID log_stats(CSocket* socket);