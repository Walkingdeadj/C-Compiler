#pragma once
#include "tools.h"
#include "token.h"
#include "charinfo.h"
#include "charstream.h"

//�﷨���ڵ����
struct ASTNode
{
    const Token* token=nullptr;   //����ȷ���ö��������кŵ�Token,��������������
    
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
    
    
    //�ڵ����ڵ��ļ���
    const string& get_filename()const {
        check_token();
        return token->pos.stream->filename;
    }

    //�ڵ����ڵ��к�
    int get_line_no()const {
        check_token();
        return token->pos.line_no;
    }

    virtual void print_semantic_info(int level, ostream& os = cout)const {
        os << string(level * 4, ' ');
        os << "Line " << setw(4) << get_line_no() << ": ";
    }
    
};

