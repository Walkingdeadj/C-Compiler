#include "charstream.h"
#include "token.h"

vector<Token> tokens;

const string& Token::get_filename()const {
	return pos.stream->filename;
}

ostream& operator<<(ostream& os, const Token& t) {
	t.print(os);
	return os;
}

void Token::print(ostream& os) const
{
	pos.print_info(os);
	os << " Token "
		<< setw(3)
		<< right
		<< type
		<< " Text "
		<< lexme
		;
}


//检查某个Token是否具有指定类型
void check_type(const Token* t, TokenType type) {
	assert(t != nullptr && t->match(type));
}

//检查某个Token是否具有指定类型
void check_type(const Token* t, const set<TokenType>& types) {
	assert(t != nullptr && t->match(types));
}

//一元运算符列表
static const set<TokenType> unary_ops = {
	INC,DEC,MINUS,PLUS,BIT_NOT,NOT
};

//二元运算符列表
static const set<TokenType> bin_ops = {
	PLUS,MINUS,STAR,DIV,PERC
	,ASSGIN,PLUS_ASSIGN,MINUS_ASSIGN,MUL_ASSIGN,DIV_ASSIGN
	,LT,GT,LE,GE
	,EQ,NE
	,OR,AND
	,BIT_AND,BIT_OR
	,LP,LBRACKET,DOT,COMMA
};

void check_op(const Token* t) {
	assert(t != nullptr);
	assert(t->match(unary_ops) || t->match(bin_ops) || t->match(QN_MARK));
}

void check_unary_op(const Token* t) {
	check_type(t, unary_ops);
}
void check_bin_op(const Token* t) {
	check_type(t, bin_ops);
}
