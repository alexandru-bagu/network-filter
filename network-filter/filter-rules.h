#pragma once
#include "CSocket.h"

const INT64 BYTE_UNIT = 1;
const INT64 KILOBYTE = 1024 * BYTE_UNIT;
const INT64 MEGABYTE = 1024 * KILOBYTE;
const INT64 GIGABYTE = 1024 * MEGABYTE;

BOOL remote_endpoint_is_blocked(CSocket* socket);
BOOL socket_check_recv_stats(CSocket* socket);
BOOL socket_check_send_stats(CSocket* socket);
