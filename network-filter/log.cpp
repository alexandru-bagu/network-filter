#include "pch.h"
#include "log.h"
#include <fstream>
#include <vector>
#include "format.h"

std::ofstream* ofs = nullptr;
MUTEX _logSyncRoot;

const int padding_width = 22;

std::vector<STRING> column_names = { STRING("Id") , STRING("Endpoint"), STRING("Recv KB/s"), STRING("Avg Recv KB/s"), STRING("Send KB/s"), STRING("Avg Send KB/s") };
std::vector<INT32> column_max_sizes = { 6, 48, 16, 16, 16, 16 };

const STRING ver_sep = " | ";
const STRING hor_sep = STRING(padding_width, '-');

STRING& pad(STRING& str, const size_t num, const char paddingChar = ' ')
{
	if (num > str.size())
		str.insert(0, num - str.size(), paddingChar);
	return str;
}

//must be called in a synchronized block
VOID log_line() {
	if (ofs != nullptr) {
		for (UINT32 i = 0; i < column_max_sizes.size(); i++)
			*ofs << STRING(column_max_sizes[i], '-') << ver_sep;
		*ofs << std::endl;
	}
}

VOID log_open()
{
	LOCK(_logSyncRoot,
		if (ofs == nullptr) {
			ofs = new std::ofstream("log.txt", std::ofstream::trunc);
			ofs->precision(2);
			for (UINT32 i = 0; i < column_names.size(); i++)
				*ofs << pad(column_names[i], column_max_sizes[i]) << ver_sep;
			*ofs << std::endl;
			log_line();
		}
	)
}

VOID log_stats(CSocket* socket)
{
	if (ofs != nullptr) {
		STRING identifier = std::to_string(socket->Identifier());
		STRING endpoint = socket->Address() + STRING(":") + std::to_string(socket->Port());
		STRING dlSpeed = net_speed_format(socket->CurrentDownloadSpeed());
		STRING avgDlSpeed = net_speed_format(socket->AverageDownloadSpeed());
		STRING ulSpeed = net_speed_format(socket->CurrentUploadSpeed());
		STRING avgUlSpeed = net_speed_format(socket->AverageUploadSpeed());

		LOCK(_logSyncRoot,
			if (ofs != nullptr) {
				*ofs
					<< pad(identifier, column_max_sizes[0]) << ver_sep
					<< pad(endpoint, column_max_sizes[1]) << ver_sep
					<< pad(dlSpeed, column_max_sizes[2]) << ver_sep
					<< pad(avgDlSpeed, column_max_sizes[3]) << ver_sep
					<< pad(ulSpeed, column_max_sizes[4]) << ver_sep
					<< pad(avgUlSpeed, column_max_sizes[5]) << ver_sep
					<< std::endl;
			}
		)
	}
}

VOID log_close()
{
	LOCK(_logSyncRoot,
		if (ofs != nullptr) {
			ofs->close();
			delete ofs;
			ofs = nullptr;
		}
	);
}