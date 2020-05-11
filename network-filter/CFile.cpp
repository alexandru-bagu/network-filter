#include "pch.h"
#include "CFile.h"
#include "path.h"

CFile::CFile() {
	this->_open = false;
	this->_syncRoot = new MUTEX;
}

CFile::~CFile() {
	delete this->_syncRoot;
}

MUTEX* CFile::SyncRoot() {
	return this->_syncRoot;
}

BOOL CFile::IsOpen()
{
	return this->_open;
}

VOID CFile::Open(STRING path, BOOL truncate) {
	BEGIN_LOCK_PTR(this->SyncRoot());
	{
		if (this->IsOpen()) {
			return;
		}
		this->_open = true;
		auto flag = CFILE_TRUNCATE;
		if (!truncate) flag = CFILE_APPEND;
		this->_file = OUTPUT_FILE(path, flag);
		this->_file.precision(2);
	}
	END_LOCK(this->_syncRoot);
}

VOID CFile::Append(STRING text) {
	BEGIN_LOCK_PTR(this->SyncRoot());
	{
		if (!this->IsOpen()) {
			return;
		}

		this->_file << text;
	}
	END_LOCK(this->_syncRoot);
}

VOID CFile::AppendLine(STRING text) {
	BEGIN_LOCK_PTR(this->SyncRoot());
	{
		if (!this->IsOpen()) {
			return;
		}
		this->_file << text << std::endl;
	}
	END_LOCK(this->_syncRoot);
}

VOID CFile::Close() {
	BEGIN_LOCK_PTR(this->SyncRoot());
	{
		if (!this->IsOpen()) {
			return;
		}
		this->_open = false;
		this->_file.close();
	}
	END_LOCK(this->_syncRoot);
}