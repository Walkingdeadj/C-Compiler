#pragma once
#include "tools.h"
#include "token.h"

//�ʷ�������,Ϊ�﷨����������
struct TokenStream
{
    //��ȡ��һ������д��token,���ı��дλ��
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

    int token_index = 0; //��һ������д��token���±�
};
