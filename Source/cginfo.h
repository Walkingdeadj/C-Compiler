#pragma once
#include "tools.h"

//some help information for code generation
struct CondStatement;
struct CgInfo
{
    int label_no=0; //�����п��ƽṹ�ı��

    void reset_label() {
        label_no =0;
    }

    int new_label() {
        return ++label_no;
    }
    vector<const CondStatement*> loop_stack;
};

extern CgInfo* cginfo;

