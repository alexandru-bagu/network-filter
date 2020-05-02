#pragma once
#include "CSocket.h"
#include "lock.h"
#include "format.h"
#include <fstream>
#include <vector>

typedef std::ofstream OUTPUT_FILE;
#define TRUNCATE OUTPUT_FILE::trunc

class CSocketLog
{
private:
	MUTEX _syncRoot;
	OUTPUT_FILE _file;
	BOOL _open;

	///must be called in a synchronized block
	VOID line();
public:
	CSocketLog();

	VOID Open();
	VOID Append(CSocket*);
	VOID Close();
};