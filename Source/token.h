#pragma once
#include "tokentype.h"
#include "charinfo.h"
// 实现词法对象的类
struct Token {
	string lexme;    //词法对象的内容
	TokenType type;  //词法对象的种类编号


	bool match(TokenType type)const {
		return this->type == type;
	}

	bool match(const set<TokenType>& types)const {
		return types.find(type) != types.end();
	}

	//下面是输出时相关的位置信息
	CharInfo pos; //首字符的位置信息

	const string& get_filename()const;

	const int get_line_no()const {
		return pos.line_no;
	}

	Token() {

	}

	// 拷贝构造函数
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

	// 输出词法对象的具体信息,用于part1 和part2
	void print(ostream& os = cout)const;

	//是否可以作为变量类型名
	bool is_var_type()const {
		return type == TYPE && lexme != "void";
	}

	//是否可以作为函数的返回类型名
	bool is_fun_type()const {
		return type == TYPE;
	}
};

// 重载 <<运算符
ostream& operator<<(ostream& os, const Token& token);

// 生成的词法对象序列,为语法分析的基础
extern vector<Token> tokens;

void check_type(const Token* t, TokenType type); //检查某个Token是否具有指定类型
void check_type(const Token* t, const set<TokenType>& types); //检查某个Token是否具有指定类型

void check_op(const Token* t);
void check_unary_op(const Token* t);
void check_bin_op(const Token* t);