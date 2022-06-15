#pragma once
#include "tokentype.h"
#include "charinfo.h"
// ʵ�ִʷ��������
struct Token {
	string lexme;    //�ʷ����������
	TokenType type;  //�ʷ������������


	bool match(TokenType type)const {
		return this->type == type;
	}

	bool match(const set<TokenType>& types)const {
		return types.find(type) != types.end();
	}

	//���������ʱ��ص�λ����Ϣ
	CharInfo pos; //���ַ���λ����Ϣ

	const string& get_filename()const;

	const int get_line_no()const {
		return pos.line_no;
	}

	Token() {

	}

	// �������캯��
	Token(const Token& other) :
		lexme(other.lexme)
		, type(other.type)
		, pos(other.pos)
	{

	}

	Token& operator=(const Token& other) {
		if (this != &other) {
			this->pos = other.pos;
			this->type = other.type;
			this->lexme = other.lexme;
		}
		return *this;
	}

	// ����ʷ�����ľ�����Ϣ,����part1 ��part2
	void print(ostream& os = cout)const;

	//�Ƿ������Ϊ����������
	bool is_var_type()const {
		return type == TYPE && lexme != "void";
	}

	//�Ƿ������Ϊ�����ķ���������
	bool is_fun_type()const {
		return type == TYPE;
	}
};

// ���� <<�����
ostream& operator<<(ostream& os, const Token& token);

// ���ɵĴʷ���������,Ϊ�﷨�����Ļ���
extern vector<Token> tokens;

void check_type(const Token* t, TokenType type); //���ĳ��Token�Ƿ����ָ������
void check_type(const Token* t, const set<TokenType>& types); //���ĳ��Token�Ƿ����ָ������

void check_op(const Token* t);
void check_unary_op(const Token* t);
void check_bin_op(const Token* t);