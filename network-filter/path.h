#pragma once
#include "pch.h"
#include <vector>

STRING app_full_name();
STRING app_name();
STRING app_path();
STRING app_data_path();
STRING absolute_path(STRING relative);
STRING get_directory_name(STRING path);
STRING get_file_name(STRING path);
STRING get_file_name_without_extension(STRING path);
STRING combine(STRING path1, STRING path2);
STRING combine(std::vector<STRING> paths);
VOID ensure_directory_exists(STRING absolute_path);