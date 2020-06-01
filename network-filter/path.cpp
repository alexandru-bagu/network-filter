#include "pch.h"
#include "path.h"
#include <shlwapi.h>
#include <shlobj.h>
#define BUFSIZE 4096

STRING path::app_full_name() {
	CHAR szPath[MAX_PATH];
	GetModuleFileNameA(0, szPath, MAX_PATH);
	return STRING(szPath);
}

STRING path::app_path() {
	return directory_name(app_full_name());
}

STRING path::app_name() {
	return file_name(app_full_name());
}

STRING path::app_data_path() {
	CHAR szPath[MAX_PATH];
	STRING app_data_path;
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
		app_data_path = STRING(szPath);
	else
		app_data_path = STRING(".");

	return combine(app_data_path, STRING("network-filter"));
}

STRING path::absolute_path(STRING relative)
{
	CHAR szPath[BUFSIZE];
	CHAR** lppPart = { NULL };
	GetFullPathNameA(relative.c_str(), (DWORD)relative.size(), szPath, lppPart);
	return STRING(szPath);
}

STRING path::directory_name(STRING path)
{
	size_t found;
	found = path.find_last_of("/\\");
	return path.substr(0, found);
}

STRING path::file_name(STRING path)
{
	size_t found;
	found = path.find_last_of("/\\");
	if (found) return path.substr(found + 1);
	return path;
}

STRING path::file_name_without_extension(STRING path)
{
	STRING file_name = path::file_name(path);
	size_t found;
	found = path.find_last_of(".");
	if (found) return path.substr(0, found);
	return path;
}

BOOL path::file_exists(STRING directory)
{
	DWORD ftyp = GetFileAttributesA(directory.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (!(ftyp & FILE_ATTRIBUTE_DIRECTORY))
		return true;   // this is not a directory!

	return false;    // this is a directory!
}

BOOL path::directory_exists(STRING directory)
{
	DWORD ftyp = GetFileAttributesA(directory.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

VOID path::ensure_directory_exists(STRING absolute_path) {
	if (!path::directory_exists(absolute_path)) {
		SECURITY_ATTRIBUTES saPermissions;

		saPermissions.nLength = sizeof(SECURITY_ATTRIBUTES);
		saPermissions.lpSecurityDescriptor = NULL;
		saPermissions.bInheritHandle = TRUE;
		CreateDirectoryA(absolute_path.c_str(), &saPermissions);
	}
}

STRING path::combine(STRING path1, STRING path2) {
	std::vector<STRING> paths = { path1, path2 };
	return combine(paths);
}

STRING path::combine(STRING path1, STRING path2, STRING path3) {
	std::vector<STRING> paths = { path1, path2, path3 };
	return combine(paths);
}

STRING path::combine(STRING path1, STRING path2, STRING path3, STRING path4) {
	std::vector<STRING> paths = { path1, path2, path3, path4 };
	return combine(paths);
}

STRING path::combine(STRING_VECTOR paths)
{
	if (paths.size() == 0) {
		return STRING("");
	}
	CHAR szPath1[BUFSIZE], szPath2[BUFSIZE];
	strcpy_s(szPath1, paths[0].c_str());
	for (UINT32 i = 1; i < paths.size(); i++) {
		PathCombineA(szPath2, szPath1, paths[i].c_str());
		strcpy_s(szPath1, szPath2);
	}
	return STRING(szPath1);
}
