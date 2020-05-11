#pragma once
#include "lock.h"
#include "format.h"
#include <fstream>
#include <vector>

typedef std::ofstream OUTPUT_FILE;
#define CFILE_APPEND OUTPUT_FILE::app
#define CFILE_TRUNCATE OUTPUT_FILE::trunc

class CFile
{
private:
	MUTEX* _syncRoot;
	OUTPUT_FILE _file;
	BOOL _open;
protected:
	MUTEX* SyncRoot();
public:
	CFile();
	~CFile();

	BOOL IsOpen();
	VOID Open(STRING, BOOL);
	VOID Append(STRING);
	VOID AppendLine(STRING);
	VOID Close();
};