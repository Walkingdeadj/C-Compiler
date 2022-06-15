#include "tokentype.h"
// 合法的单字符列表
const string single_chars_symbols = "!%&()*+,-./:;<=>?[]{|}~";

// 含2个字符的运算符
const string op2s[] = {
	"=="
	,"!="
	,">="
	,"<="
	,"++"
	,"--"
	,"||"
	,"&&"
	,"+="
	,"-="
	,"*="
	,"/="
};

// 关键字列表
const string keywords[] = {
	"const"
	,"struct"
	,"for"
	,"while"
	,"do"
	,"if"
	,"else"
	,"break"
	,"continue"
	,"return"
	,"switch"
	,"case"
	,"default"
};

/* 存储每个长度为2的运算符的开头字符,
 * 即 '+','-','*','/','='等
 * 只有这些字符时,说明词法对象的长度有可能为2,此时需要继续读取下一个字符,
 *
 */
set<char> op2_start;

// 每个单字符对象的编号
map<string, TokenType> op1_type;

// 每个长度为2运算符对应的编号
map<string, TokenType> op2_type;

// 每个关键字对应的编号
map<string, TokenType> keyword_type;

const set<TokenType> mul_ops = { STAR,DIV,PERC };
const set<TokenType> add_ops = { PLUS,MINUS };
const set<TokenType> relation_ops = { LE,GT,LT,GE };
const set<TokenType> eq_ops = { NE,EQ };
const set<TokenType> assign_ops = { ASSGIN,PLUS_ASSIGN,MINUS_ASSIGN
,MUL_ASSIGN,DIV_ASSIGN };

const set<TokenType> type_start = { STRUCT,TYPE,CONST };


const set<TokenType> term_start = { ID,INT_LIT,CHAR_LIT,REAL_LIT,STRING_LIT,LP };
const set<TokenType> expr_start = { MINUS,NOT,BIT_NOT,ID,INT_LIT,CHAR_LIT,REAL_LIT,STRING_LIT,LP,INC,DEC };

extern const set<TokenType> stat_start = {
	LBRACE,MINUS,NOT,BIT_NOT,ID,INT_LIT,CHAR_LIT,REAL_LIT,STRING_LIT,LP,INC,DEC
	,SEMICOLON
	,RETURN
	,BREAK
	,CONTINUE
	,IF
	,DO
	,WHILE
	,FOR
};

//测试函数
void token_type_test1() {
	{
		size_t n1 = single_chars_symbols.size();
		size_t n2 = op1_type.size();
		assert(n1 == n2);

		char c = ';';
		assert(is_op1(c));
		TokenType t = get_op1_type(c);
		assert(t == SEMICOLON);
	}

	{
		assert(!is_op1('#'));
		assert(!is_op1('@'));
	}

	{
		size_t n = op2_type.size();
		assert(n == 12);

		assert(is_op2_start('+'));
		assert(is_op2("++"));
	}

	{
		const string s = "&=";
		assert(!is_op2(s));
	}

	{
		const string s = "==";
		assert(is_op2(s));
		TokenType t = get_op2_type(s);
		assert(t == EQ);
	}


	{
		const string s = "+=";
		assert(is_op2(s));
		TokenType t = get_op2_type(s);
		assert(t == PLUS_ASSIGN);
	}

	{
		const string s = "/=";
		assert(is_op2(s));
		TokenType t = get_op2_type(s);
		assert(t == DIV_ASSIGN);
	}

	{
		size_t n = keyword_type.size();
		assert(n == 13);
	}

	{
		const string s = "consT";
		assert(!is_keyword(s));
	}

	{
		const string s = "const";
		assert(is_keyword(s));
		TokenType t = get_keyword_type(s);
		assert(t == CONST);
	}

	{
		const string s = "else";
		assert(is_keyword(s));
		TokenType t = get_keyword_type(s);
		assert(t == ELSE);
	}

	{
		const string s = "default";
		assert(is_keyword(s));
		TokenType t = get_keyword_type(s);
		assert(t == DEFAULT);
	}

}


/*
 建立好需要的表,即op1_type,op2_type,keyword_type,以便加快查询
*/
void init_tokentype_table() {
	for (size_t i = 0;i < single_chars_symbols.size();++i) {
		char c = single_chars_symbols[i];
		string s(1, c);
		op1_type[s] = (TokenType)((int)(c));
	}

	for (int i = 0, n = 351;i < 8;++i, ++n) {
		const string& s = op2s[i];
		op2_type[s] = (TokenType)(n);
		op2_start.insert(s[0]);
	}

	for (int i = 8, n = 361;i < 12;++i, ++n) {
		const string& s = op2s[i];
		op2_type[s] = (TokenType)(n);
		op2_start.insert(s[0]);
		assert(is_op1(s[0]));
	}

	for (int i = 0, n = 401;i < 13;++i, ++n) {
		const string& s = keywords[i];
		keyword_type[s] = (TokenType)(n);
	}
}

// 判断某个单词是否为关键字
bool is_keyword(const string& s) {
	return keyword_type.find(s) != keyword_type.end();
}

// 判断某个字符是否为运算符的开头
bool is_op2_start(char c) {
	return op2_start.find(c) != op2_start.end();
}

// 判断某个长度为2的单词是否为运算符
bool is_op2(const string& s) {
	return op2_type.find(s) != op2_type.end();
}

bool is_op1(char c) {
	string s(1, c);
	return op1_type.find(s) != op1_type.end();
}

// 获取单字符对应的编号
TokenType get_op1_type(char c) {
	string s(1, c);
	TokenType t = op1_type.find(s)->second;
	assert(t == (TokenType)((int)(c)));
	return t;
}

// 获取运算符对应的编号
TokenType get_op2_type(const string& s) {
	TokenType t = op2_type.find(s)->second;
	return t;
}

bool is_type_keyword(const string& s) {
	return  (s == "void" || s == "int" || s == "float" || s == "char");
}

// 获取关键字的编号
TokenType get_keyword_type(const string& s) {
	TokenType t = keyword_type.find(s)->second;
	return t;
}
