#pragma once
#include "CFile.h"

class CLog : public CFile
{
protected:
	VOID Append(STRING, STRING);
public:
	CLog();
	VOID Debug(STRING);
	VOID Trace(STRING);
	VOID Info(STRING);
	VOID Error(STRING);
};