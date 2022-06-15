#pragma once
#include "tools.h"
#include "macro.h"

//������������ָ��
struct IfDirective
{
    const string filename;
    const int line_no;

    int else_line_no = 0;

    const string macro_name;  //��صĺ���
    const string dir_name;  //ָ����

    bool in_else_branch = false;   // �Ƿ��Ѿ�����else��֧

    const bool condition_value;  //��ָ����������ļ�����
    const bool father_expand;   // ��¼���׵�need_expand�ļ�����

    // �������������ֵ�����Ƿ�ִ�б���֧
    bool run_current_branch()const {
        return in_else_branch != condition_value;
    }

    /*�Ƿ�չ����ָ�����֧�ڵĴʷ�����
    ֻ�и��׵�expandֵΪ���ҿ���ִ�б���֧ʱ
    �Ž��дʷ������Ĵ���
    */
    bool need_expand()const {
        return father_expand && run_current_branch();
    }

    IfDirective(const string filename, int line_no, const string& macro_name, const string& dir_name) :
        filename(filename)
        , line_no(line_no)
        , macro_name(macro_name)
        , dir_name(dir_name)
        , condition_value((dir_name == "ifdef") == is_macro_exists(macro_name))
        , father_expand(true)
    {
        if (debug_flag[2]) {
            print();
        }
    }

    IfDirective(const string filename, int line_no, const string& macro_name, const string& dir_name, IfDirective* father) :
        filename(filename)
        , line_no(line_no)
        , macro_name(macro_name)
        , dir_name(dir_name)
        , condition_value((dir_name == "ifdef") == is_macro_exists(macro_name))
        , father_expand(father->need_expand()) {
        if (debug_flag[2]) {
            print();
        }
    }

    void print(ostream& os = cout)const {
        os << dir_name << " at " << line_no
            << " ,condition: " << boolalpha << condition_value
            ;
        if (in_else_branch) {
            os << "#else at line " << else_line_no;
            os << " now in else branch";
        }
        else
        {
            os << " now in first branch";
        }

        os << "expand value: " << boolalpha << need_expand();
        os << endl;
    }
};

extern stack<IfDirective*> if_dire_stack;

ostream& operator<<(ostream& os, const IfDirective& dire);
