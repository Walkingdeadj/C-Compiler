#pragma once
#include "tools.h"
#include "macro.h"

//处理条件编译指令
struct IfDirective
{
    const string filename;
    const int line_no;

    int else_line_no = 0;

    const string macro_name;  //相关的宏名
    const string dir_name;  //指令名

    bool in_else_branch = false;   // 是否已经进入else分支

    const bool condition_value;  //本指令入口条件的计算结果
    const bool father_expand;   // 记录父亲的need_expand的计算结果

    // 根据入口条件的值决定是否执行本分支
    bool run_current_branch()const {
        return in_else_branch != condition_value;
    }

    /*是否展开本指令各分支内的词法分析
    只有父亲的expand值为真且可以执行本分支时
    才进行词法分析的处理
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
