#include "pch.h"
#include "filter-rules.h"
#include "format.h"

BOOL endpoint_is_blocked(CSocket* socket)
{
	return true;
}

BOOL socket_check_recv_stats(CSocket* socket)
{
	/*DOUBLE dlSpeed = socket->AverageDownloadSpeed();
	STRING fmt = net_speed_format(dlSpeed);
	if (socket->Reliable() && dlSpeed > 15 * MEGABYTE) {
		printf("%s", fmt.c_str());
		return false;
	}*/
	return false;
}

BOOL socket_check_send_stats(CSocket* socket)
{
	return false;
}