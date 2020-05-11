#include "pch.h"
#include "path.h"
#include <sstream>
#include "CSocketLog.h"

std::vector<STRING> column_names = { STRING("Id") , STRING("Local"), STRING("Remote"), STRING("Recv KB/s"), STRING("Avg Recv KB/s"), STRING("Send KB/s"), STRING("Avg Send KB/s") };
std::vector<INT32> column_max_sizes = { 6, 46, 46, 12, 14, 12, 14 };
STRING vertical_separator = " | ";
STRING output_name = "stats.log";

STRING& pad(STRING& str, const size_t num, const char paddingChar = ' ')
{
	if (num > str.size())
		str.insert(0, num - str.size(), paddingChar);
	return str;
}

CSocketLog::CSocketLog() {
	::CFile();
}

VOID CSocketLog::line() {
	if (!this->IsOpen()) {
		return;
	}
	std::ostringstream ostr;
	for (UINT32 i = 0; i < column_max_sizes.size(); i++)
		ostr << STRING(column_max_sizes[i], '-') << vertical_separator;
	this->AppendLine(ostr.str());
}

VOID CSocketLog::Open() {
	CFile::Open(path::combine(path::app_data_path(), path::file_name_without_extension(path::app_name()) + ".txt"), TRUE);
	if (this->IsOpen()) {
		std::ostringstream ostr;
		for (UINT32 i = 0; i < column_names.size(); i++)
			ostr << pad(column_names[i], column_max_sizes[i]) << vertical_separator;
		CFile::AppendLine(ostr.str());
		this->line();
	}
}

VOID CSocketLog::Append(CSocket* socket) {
	STRING identifier = std::to_string(socket->Identifier());
	STRING localEndpoint = socket->LocalEndpoint();
	STRING remoteEndpoint = socket->RemoteEndpoint();
	STRING dlSpeed = net_speed_format(socket->CurrentDownloadSpeed());
	STRING avgDlSpeed = net_speed_format(socket->AverageDownloadSpeed());
	STRING ulSpeed = net_speed_format(socket->CurrentUploadSpeed());
	STRING avgUlSpeed = net_speed_format(socket->AverageUploadSpeed());
	std::ostringstream ostr;

	ostr << pad(identifier, column_max_sizes[0]) << vertical_separator
		<< pad(localEndpoint, column_max_sizes[1]) << vertical_separator
		<< pad(remoteEndpoint, column_max_sizes[2]) << vertical_separator
		<< pad(dlSpeed, column_max_sizes[3]) << vertical_separator
		<< pad(avgDlSpeed, column_max_sizes[4]) << vertical_separator
		<< pad(ulSpeed, column_max_sizes[5]) << vertical_separator
		<< pad(avgUlSpeed, column_max_sizes[6]) << vertical_separator;
	CFile::AppendLine(ostr.str());
}