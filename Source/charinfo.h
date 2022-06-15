#pragma once
#include "tools.h"
//字符对象的具体信息,包括内容,来源,位置等
struct CharStream;
struct CharInfo
{
    //字符的行号和列号,仅在part1中有效
    int line_no = 1; //对part2, 如果输入流来自宏展开, 则这个行号代表宏展开发生时的行号
    int col_no = 1;

    char value = 0;  //字符的具体值

    mutable CharStream* stream = nullptr; //记录本字符来源于哪个输入流

    //无参构造函数
    CharInfo() {

    }

    //拷贝构造函数
    CharInfo(const CharInfo& other) :
        line_no(other.line_no)
        , col_no(other.col_no)
        , value(other.value)
        , stream(other.stream)
    {

    }

    CharInfo& operator=(const CharInfo& other) {
        if (this != &other) {
            this->line_no = other.line_no;
            this->col_no = other.col_no;
            this->value = other.value;
            this->stream = other.stream;
        }
        return *this;
    }

    void print(ostream& os = cout)const;

    // 只输出来源信息
    void print_info(ostream& os = cout)const;

};

ostream& operator<<(ostream& os, const CharInfo& pos);

