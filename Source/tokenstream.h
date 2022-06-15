#pragma once
#include "tools.h"
#include "token.h"

//词法流的类,为语法分析的输入
struct TokenStream
{
    //读取下一个待读写的token,并改变读写位置
    const Token* move_next() {
        assert(token_index >= 0 && token_index < (int)(tokens.size()));
        int index = token_index;
        ++token_index;
        auto p = &(tokens[index]);
        if (debug_flag[3]) {
            cout << endl << "get token at index " << index << " : " << (p->lexme) << endl;
            cout << "next index=" << token_index << endl;
        }
        return p;
    }

    bool has_next()const {
        return token_index < (int)(tokens.size());
    }

    void unget(const Token* t) {
        if (debug_flag[3]) {
            cout << endl << "unget token : " << (t->lexme) << endl;
            cout << "index=" << token_index << endl;
        }

        assert(token_index > 0);
        --token_index;

        assert(t == &tokens[token_index]);
    }

    int token_index = 0; //下一个待读写的token的下标
};
