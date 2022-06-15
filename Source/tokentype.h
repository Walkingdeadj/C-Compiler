#pragma once
#include "tools.h"

// 各种词法对象的编号
// 以part1的要求为准
enum  TokenType
{
	// 单个字符的编号就是其ascii码
	UNKNOWN = 0  // 未知的token
	, NOT = '!'
	, PERC = '%'
	, BIT_AND = '&'
	, LP = '('
	, RP = ')'
	, STAR = '*'
	, PLUS = '+'
	, COMMA = ','
	, MINUS = '-'
	, DOT = '.'
	, DIV = '/'
	, COLON = ':'
	, SEMICOLON = ';'
	, LT = '<'
	, ASSGIN = '='
	, GT = '>'
	, QN_MARK = '?'
	, LBRACKET = '['
	, RBRACKET = ']'
	, LBRACE = '{'
	, RBRACE = '}'
	, BIT_OR = '|'
	, BIT_NOT = '~'

	//下面的编号见part1中的表
	, TYPE = 301
	, CHAR_LIT = 302
	, INT_LIT = 303
	, REAL_LIT = 304
	, STRING_LIT = 305
	, ID = 306

	, EQ = 351
	, NE = 352
	, GE = 353
	, LE = 354
	, INC = 355
	, DEC = 356
	, OR = 357
	, AND = 358
	, PLUS_ASSIGN = 361
	, MINUS_ASSIGN = 362
	, MUL_ASSIGN = 363
	, DIV_ASSIGN = 364

	, CONST = 401
	, STRUCT = 402
	, FOR = 403
	, WHILE = 404
	, DO = 405
	, IF = 406
	, ELSE = 407
	, BREAK = 408
	, CONTINUE = 409
	, RETURN = 410
	, SWTICH = 411
	, CASE = 412
	, DEFAULT = 413
	,END_OF_INPUT=500
};

void init_tokentype_table();

// 判断是否为类型名的关键字
bool is_type_keyword(const string& s);

bool is_keyword(const string& s);
bool is_op2(const string& s);
bool is_op1(char c);
bool is_op2_start(char c);

TokenType get_op1_type(char c);
TokenType get_op2_type(const string& s);
TokenType get_keyword_type(const string& s);

void token_type_test1();

extern const set<TokenType> mul_ops;
extern const set<TokenType> add_ops;
extern const set<TokenType> relation_ops;
extern const set<TokenType> eq_ops;
extern const set<TokenType> assign_ops;

extern const set<TokenType> type_start;

extern const set<TokenType> term_start;
extern const set<TokenType> expr_start;
extern const set<TokenType> stat_start;

