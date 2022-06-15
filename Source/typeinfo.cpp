#include "typeinfo.h"

string SemanticTypeInfo::get_utf() const
{
    ostringstream oss;
    string name =type_name;
    if (is_array) {
        oss<<"[";
    }
    oss <<(char)(toupper(name[0]));
    return oss.str();
}

string SemanticTypeInfo::str() const
{
    ostringstream oss;
    if (is_const)
        oss << "const ";
    if (type == STRUCT)
        oss << "struct ";
    oss << type_name;
    if (is_array)
        oss << "[]";
    return oss.str();
}
