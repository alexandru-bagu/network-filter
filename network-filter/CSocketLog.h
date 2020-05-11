#pragma once
#include "CFile.h"
#include "CSocket.h"

class CSocketLog : public CFile
{
private:
	///must be called in a synchronized block
	VOID line();
public:
	CSocketLog();

	VOID Open();
	VOID Append(CSocket*);
};