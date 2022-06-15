#include "symbol_table.h"
#include "decl.h"

const TypedSymbolDecl* SymbolTable::find_id(const string& name) const
{
    for (const auto e : *this) {
        if (e->name->lexme == name) {
            const auto p = dynamic_cast<const VarDecl*> (e);
            if (p != nullptr)
                return p;
            const auto p2 = dynamic_cast<const ArrayDecl*> (e);
            if (p2 != nullptr)
                return p2;
        }
    }
    return nullptr;
}

// �ڱ����ű��в��ұ��������߲�����
const VarDecl* SymbolTable::find_var(const string& name)const {
    return find_symbol<VarDecl>(name);
}

// �ڱ����ű��в���������
const ArrayDecl* SymbolTable::find_array(const string& name)const {
    return find_symbol<ArrayDecl>(name);
}

// �ڱ����ű��в��Һ�����
const FunDecl* SymbolTable::find_fun(const string& name)const {
    return find_symbol<FunDecl>(name);
}

// �ڱ����ű��в��ҽṹ����
const StructDecl* SymbolTable::find_struct(const string& name) {
    return find_symbol<StructDecl>(name);
}

template<class T>
const T* SymbolTable::find_symbol(const string& name)const {
    for (const auto e : *this) {
        if (e->name->lexme == name) {
            const auto p = dynamic_cast<const T*> (e);
            if (p != nullptr)
                return p;
        }
    }
    return nullptr;
}