#include "macro.h"
#include "charstream.h"

void CharStream::update_position(char c)
{
    //更新下一个待读取字符的行号和列号
    if (c == '\n') {
        ++pos.line_no;
        pos.col_no = 1;
    }
    else if (c == '\t')
        pos.col_no += 4;
    else
        ++pos.col_no;
}

char CharStream::pop(CharInfo& pos)
{
    if (!buffer.empty()) {

        pos = buffer.top();
        buffer.pop();

        if (debug_flag[1]) {
            cout << "read from buffer:"
                << pos << endl;
        }

    }
    else {
        char c = is->get();

        //复制位置信息
        pos = this->pos;
        pos.value = c;
        pos.stream = this;

        // 更新行号和列号
        if (!is_macro_expand())
            update_position(c);


        if (debug_flag[1]) {
            cout << "read from stream:"
                << pos << endl;
        }
    }
    return pos.value;
}

bool CharStream::has_next() const {
    return (!buffer.empty() || is->peek() != EOF);
}

bool CharStream::has_include(const string& filename) const
{
    const CharStream* p = this;
    while (p != nullptr) {
        if (!(p->is_macro_expand()) && p->filename == filename)
            return true;
        p = p->father;
    }
    return false;
}

bool CharStream::has_expand(const string& macroname) const
{
    const CharStream* p = this;
    while (p != nullptr) {
        if (p->is_macro_expand() && p->macroname == macroname)
            return true;
        p = p->father;
    }
    return false;
}

//一些测试函数
void char_stream_test1() {
    assert(file_exists("1.txt"));
    CharStream input("1.txt");
    assert(!input.has_next());
}

void char_stream_test2() {
    assert(file_exists("2.txt"));
    CharStream input("2.txt");
    while (input.has_next()) {
        CharInfo pos;
        input.pop(pos);
        cout << pos << endl;
    }
}
