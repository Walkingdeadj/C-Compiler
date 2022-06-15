#pragma once
#include "tools.h"
struct SymbolDecl;
struct VarDecl;
struct ArrayDecl;
struct FunDecl;
struct StructDecl;
struct TypedSymbolDecl;
struct SymbolTable :public vector<SymbolDecl*> {
    template<class T>
    const T* find_symbol(const string& name)const;
    
    const TypedSymbolDecl* find_id(const string& name)const; //查找变量名或者数组名

    const VarDecl* find_var(const string& name)const;   // 在本符号表中查找变量名,或者参数名
    const ArrayDecl* find_array(const string& name)const; // 在本符号表中查找数组名或者参数名
    const FunDecl* find_fun(const string& name)const;   // 在本符号表中查找函数名
    const StructDecl* find_struct(const string& name);  // 在本符号表中查找结构体名
};
