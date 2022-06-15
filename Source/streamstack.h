#pragma once
#include "tools.h"
#include "charstream.h"
/*
输入流组成的栈,对part1,这个栈中只有一个输入流
,对part2,当遇到include语句或者宏展开时,则把
新的输入流对象放到栈顶,每次都先从栈顶中读取字符
lexer和这个栈进行交互.
对一个输入流已经到达结尾时,必须马上把它栈顶移出.
同样,只有非空的输入流才允许进栈.
*/
struct StreamStack {
    stack<CharStream*> streams;

    char pop(CharInfo& pos);  // 读取下一个字符已经其来源信息
    bool has_next(); //判断是否还有未读字符,如果已经到了流的结尾则返回false

    void clean();  //将所有为空的输入流弹出栈顶

    // 插入顶层的代码文件,只被调用一次
    void push(const string& filename);

    /*
     * 构造一个文件输入流并入栈
     * 调用者必须保证文件名已经存在
     * 本函数用于插入被include的文件
     * 仅用于part2
     */
    void push(const string& filename, CharStream* father);

    /*
     * 构造一个宏展开的输入流并入栈
     * 仅用于part2
     */
    void push(const string& macroname, int line_no, CharStream* father);
};