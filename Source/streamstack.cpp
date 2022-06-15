#include "macro.h"
#include "streamstack.h"

char StreamStack::pop(CharInfo& pos) {
    auto s = streams.top();
    char c = s->pop(pos);
    return c;
}

bool StreamStack::has_next() {
    clean();
    return  !streams.empty();
}

void StreamStack::clean()
{
    while (!streams.empty()) {
        auto s = streams.top();
        if (s->has_next())
            break;
        else {
            if (debug_flag[1] || debug_flag[2]) {
                cout << "now top lexer is empty,will pop from stack" << endl;
                cout << "top lexer info: filename " << s->filename
                    << " , macraname : " << s->macroname
                    << ", line_no" << s->pos.line_no
                    << endl;
            }
            streams.pop();
        }
    }
}

void StreamStack::push(const string& filename)
{
    auto s = new CharStream(filename);
    streams.push(s);
}

void StreamStack::push(const string& filename, CharStream* father) {
    auto s = new CharStream(filename, father);
    streams.push(s);
}

void StreamStack::push(const string& macroname
    , int line_no
    , CharStream* father) {

    auto s = new CharStream(macroname, line_no, father);
    streams.push(s);
}
