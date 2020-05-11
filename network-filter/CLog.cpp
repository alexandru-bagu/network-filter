#include "pch.h"
#include "path.h"
#include "CLog.h"
#include <sstream>
#include <chrono>
#include <ctime>   

CLog::CLog() {
	::CFile();
	this->Open(path::combine(path::app_data_path(), path::file_name_without_extension(path::app_name()) + "_log.txt"), FALSE);
}

VOID CLog::Append(STRING type, STRING text) {
	std::ostringstream ostr;
	auto now = std::chrono::system_clock::now();
	std::time_t time = std::chrono::system_clock::to_time_t(now);
	struct tm p;
	localtime_s(&p, &time);
	char buffer[64] = { 0 };
	strftime(buffer, 64, "%F %T", &p);
	ostr << "[" << buffer << "] - "<< type << ": " << text;
	CFile::AppendLine(ostr.str());
}

VOID CLog::Debug(STRING text) {
#if _DEBUG
	this->Append("DEBUG", text);
#endif
}

VOID CLog::Trace(STRING text) {
	this->Append("TRACE", text);
}

VOID CLog::Info(STRING text) {
	this->Append("INFO", text);
}

VOID CLog::Error(STRING text) {
	this->Append("ERROR", text);
}