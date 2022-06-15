#pragma once

#include "tools.h"
#include "symbol_table.h"

// ���������йص���
struct SymbolTableStack:public vector<SymbolTable*>
{
    FunDecl* current_fun = nullptr;

    SymbolTableStack() {
        push_back(new SymbolTable());
    }

    const SymbolDecl* find_dup_var(const string& name)const ; //ͬһ�������Ƿ�������������������
    const SymbolDecl* find_dup_struct(const string& name)const;   //ͬһ�������Ƿ�����������

    template<class T>
    const T* find_symbol(const string& name)const;

    const TypedSymbolDecl* find_id(const string& name)const; //���ұ���������������

    const VarDecl* find_var(const string& name)const; //�����ڲ�ķ��ű��в��ұ�����,���������߲�����
    const ArrayDecl* find_array(const string& name)const; //�ڷ���ջ�в��ұ�����,���������߲�����
    const FunDecl* find_fun(const string& name)const;
    const StructDecl* find_struct(const string& name)const;

    void add_symbol(SymbolDecl* decl) {
        const auto table_stack = *this;
        table_stack.back()->push_back(decl);
    }
};
