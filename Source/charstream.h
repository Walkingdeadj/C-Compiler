#pragma once
#include "tools.h"
#include "macro.h"

// 输入字符流的实现
// 流的内容可能来源于文件
// 也可能来源于字符串

#include "charinfo.h"

struct CharStream
{
    CharInfo pos; //下一个待读取字符的位置信息

    //用于回退的缓冲区,
    stack<CharInfo> buffer;

    //判断输入来源是否为宏展开
    bool is_macro_expand()const {
        return !macroname.empty();
    }

    // 数据成员
    int level = 0;  //输出是的缩进层次
    string filename = ""; // 输入流相关的文件名,对part2,如果输入流来自宏展开,则这个行号代表宏展开时所在的文件名
    string macroname = ""; //输入流相关的宏名,默认为空串

    istream* is = nullptr;  //输入流对象的指针,可能来源于文件,可能来源于字符串

    CharStream* father = nullptr; //父亲指针,用来判断文件嵌套和宏展开是否存在回路.

    //下面是成员函数
    CharStream(const string& filename)
        :filename(filename), is(new ifstream(filename))
    {

    }

    CharStream(const string& filename, CharStream* father)
        :level(father->level + 1)
        , filename(filename)
        , is(new ifstream(filename))
        , father(father)
    {

    }

    CharStream(const string& macroname, int line_no, CharStream* father)
        :level(father->level + 1)
        , filename(father->filename)
        , macroname(macroname)
        , is(new istringstream(get_macro_body(macroname)))
        , father(father)
    {
        if (debug_flag[2])
            cout << "push macro stream with name " << macroname
            << " in file " << filename
            << " at line " << line_no
            << endl;
        pos.line_no = line_no;
    }

    virtual ~CharStream() {
        delete is;
    }

    // 更新行号和列号,仅对part1适用
    void update_position(char c);

    void unget(const CharInfo& pos) {
        if (debug_flag[1]) {
            cout << "unget char info to buffer:";
            cout << pos << endl;
        }

        buffer.push(pos);
    }

    char pop(CharInfo& pos);  // 读取下一个字符以及其位置信息
    bool has_next()const; //判断是否还有未读字符,如果已经到了流的结尾则返回false

    //判断某个文件是否被展开了,用于防止文件自包含
    bool has_include(const string& filename)const;

    //判断某个宏是否已经被展开了,用于防止宏的自递归展开
    bool has_expand(const string& macroname)const;

};


//一些测试函数
void char_stream_test1();
void char_stream_test2();
