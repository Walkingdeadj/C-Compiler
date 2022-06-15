#pragma once
#include "tools.h"

//下面用于part2
//记录每个宏对应的字符串

bool is_macro_exists(const string& macro_name);
const string& get_macro_body(const string& macro_name);

extern map<string, string> macro_table;
