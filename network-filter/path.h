#pragma once
#include "pch.h"
#include <vector>
typedef std::vector<STRING> STRING_VECTOR;

namespace path {
	STRING app_full_name();
	STRING app_name();
	STRING app_path();
	STRING app_data_path();
	STRING absolute_path(STRING relative);
	STRING directory_name(STRING path);
	STRING file_name(STRING path);
	STRING file_name_without_extension(STRING path);
	STRING combine(STRING path1, STRING path2);
	STRING combine(STRING path1, STRING path2, STRING path3);
	STRING combine(STRING path1, STRING path2, STRING path3, STRING path4);
	STRING combine(STRING_VECTOR paths);
	VOID ensure_directory_exists(STRING absolute_path);
}