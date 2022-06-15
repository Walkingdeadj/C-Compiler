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
    
    const TypedSymbolDecl* find_id(const string& name)const; //���ұ���������������

    const VarDecl* find_var(const string& name)const;   // �ڱ����ű��в��ұ�����,���߲�����
    const ArrayDecl* find_array(const string& name)const; // �ڱ����ű��в������������߲�����
    const FunDecl* find_fun(const string& name)const;   // �ڱ����ű��в��Һ�����
    const StructDecl* find_struct(const string& name);  // �ڱ����ű��в��ҽṹ����
};
