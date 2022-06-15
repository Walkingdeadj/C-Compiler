#include "decl.h"
#include "table_stack.h"

template<class T>
const T* SymbolTableStack::find_symbol(const string& name)const {
    const auto table_stack = *this;
    assert(!table_stack.empty());
    int level_num = table_stack.size();
    for (int i = level_num - 1;i >= 0;--i) {
        const auto table = table_stack[i];
        assert(table!=nullptr);
        const auto p = table->find_symbol<T>(name);
        if (p != nullptr)
            return p;
    }
    return nullptr;
}


const SymbolDecl* SymbolTableStack::find_dup_var(const string& name) const
{
    const auto table_stack = *this;
    assert(!table_stack.empty());
    const auto table = table_stack.back();
    const SymbolDecl* p = table->find_var(name);
    if(p==nullptr)
        p= table->find_array(name);

    return p;
}

const SymbolDecl* SymbolTableStack::find_dup_struct(const string& name) const
{
    const auto table_stack = *this;
    assert(!table_stack.empty());
    const auto table = table_stack.back();
    return table->find_struct(name);
}

const TypedSymbolDecl* SymbolTableStack::find_id(const string& name) const
{
    const auto table_stack = *this;
    assert(!table_stack.empty());
    int level_num = table_stack.size();
    for (int i = level_num - 1;i >= 0;--i) {
        const auto table = table_stack[i];
        assert(table != nullptr);
        const auto p = table->find_id(name);
        if (p != nullptr)
            return p;
    }
    return nullptr;
}

const VarDecl* SymbolTableStack::find_var(const string& name) const
{
    return find_symbol<VarDecl>(name);
}

const ArrayDecl* SymbolTableStack::find_array(const string& name) const
{
    return find_symbol<ArrayDecl>(name);
}

const FunDecl* SymbolTableStack::find_fun(const string& name) const
{
    const auto table_stack = *this;
    assert(!table_stack.empty());
    const auto table = table_stack[0];
    return table->find_fun(name);
}

const StructDecl* SymbolTableStack::find_struct(const string& name) const
{
    return find_symbol<StructDecl>(name);
}




