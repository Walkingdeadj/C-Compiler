#pragma once
#include "tools.h"

//��������part2
//��¼ÿ�����Ӧ���ַ���

bool is_macro_exists(const string& macro_name);
const string& get_macro_body(const string& macro_name);

extern map<string, string> macro_table;
