#include "macro.h"

map<string, string> macro_table;

bool is_macro_exists(const string& macro_name) {
    return macro_table.find(macro_name) != macro_table.end();
}

const string& get_macro_body(const string& macro_name) {
    auto it = macro_table.find(macro_name);
    assert(it != macro_table.end());
    return it->second;
}

