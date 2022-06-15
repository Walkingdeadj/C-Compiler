#pragma once
#include "tools.h"
#include "token.h"
#include "charinfo.h"
#include "charstream.h"

//语法树节点对象
struct ASTNode
{
    const Token* token=nullptr;   //可以确定该对象所在行号的Token,由子类自行设置
    
    void set_token(const Token* t) {
        this->token = t;
    }
    
    void check_token()const {
        assert(this->token!=nullptr);
    }

    virtual void print(int level, ostream& os = cout)const {
        os << string(level * 2, ' ');
    }

    ASTNode() {
        
    }

    virtual ~ASTNode() {
        
    }
    
    
    //节点所在的文件名
    const string& get_filename()const {
        check_token();
        return token->pos.stream->filename;
    }

    //节点所在的行号
    int get_line_no()const {
        check_token();
        return token->pos.line_no;
    }

    virtual void print_semantic_info(int level, ostream& os = cout)const {
        os << string(level * 4, ' ');
        os << "Line " << setw(4) << get_line_no() << ": ";
    }
    
};

