#pragma once

#include "tools.h"
#include "symbol_table.h"

// 和语义检查有关的类
struct SymbolTableStack:public vector<SymbolTable*>
{
    FunDecl* current_fun = nullptr;

    SymbolTableStack() {
        push_back(new SymbolTable());
    }

    const SymbolDecl* find_dup_var(const string& name)const ; //同一作用域是否有重名变量或者数组
    const SymbolDecl* find_dup_struct(const string& name)const;   //同一作用域是否有重名函数

    template<class T>
    const T* find_symbol(const string& name)const;

    const TypedSymbolDecl* find_id(const string& name)const; //查找变量名或者数组名

    const VarDecl* find_var(const string& name)const; //在最内层的符号表中查找变量名,数组名或者参数名
    const ArrayDecl* find_array(const string& name)const; //在符号栈中查找变量名,数组名或者参数名
    const FunDecl* find_fun(const string& name)const;
    const StructDecl* find_struct(const string& name)const;

    void add_symbol(SymbolDecl* decl) {
        const auto table_stack = *this;
        table_stack.back()->push_back(decl);
    }
};
