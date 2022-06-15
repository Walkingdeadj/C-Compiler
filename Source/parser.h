#pragma once
#include "tools.h"
#include "token.h"
#include "tokenstream.h"
#include "expr.h"
#include "decl.h"
#include "statement.h"
#include "scope.h"
#include "symbol_table.h"

struct SymbolTableStack;

struct Parser
{
    //vector<SymbolDecl*>* program=nullptr;
    SymbolTable* program = nullptr;

    TokenStream input;  //�ʷ�Ԫ�ص�������
    const Token* last_token = nullptr; //�������һ�α���ȡ��Ԫ��,����ǰ�������next_match ����get_next����
    //TokenType last_type = END_OF_INPUT;
    
    //�����һ���ʷ�Ԫ�ص����Ͳ�����Ԥ��,�����֮
    bool next_match(TokenType type);
    bool next_match(const set<TokenType>& types);


    // �����һ���ʷ�Ԫ�ص����Ͳ�����Ԥ��,�����������Ϣ
    const Token* expect(TokenType type, char c);
    const Token* expect(TokenType type, const string& s);

    const Token* expect_semi();
    const Token* expect_id();
    const Token* expect_lp();
    const Token* expect_rp(const Token* t);
    const Token* expect_rb(const Token* t);
    const Token* expect_rc(const Token* t);
    const Token* expect_expr_start();

    // ���ʷ�Ԫ�ػ���
    void unget(const Token* t);
    bool has_next()const { return input.has_next(); }

    // ����һ��Ԫ�ر��浽last_token��,������֮
    const Token* next_token();

    //����ͱ��ʽ�����й�
    Expr* parse_expr();
    Expr* parse_expr(const Token* t);
    Expr* parse_term();
    Expr* parse_postfix_expr();
    Expr* parse_unary_expr();
    Expr* parse_cast_expr();
    Expr* parse_mul_expr();
    Expr* parse_add_expr();
    Expr* parse_shift_expr();
    Expr* parse_rel_expr();
    Expr* parse_eq_expr();

    Expr* parse_bit_and_expr();
    Expr* parse_bit_xor_expr();
    Expr* parse_bit_or_expr();

    Expr* parse_logic_and_expr();
    Expr* parse_logic_or_expr();

    Expr* parse_cond_expr();
    Expr* parse_assign_expr();

    Arguments* parse_args();


    // ��ȫ�ֶ����﷨������صĺ���
    //vector<SymbolDecl*>* do_parse();
    SymbolTable* do_parse();

    //vector<SymbolDecl*>* parse_decls(Scope scope);
    SymbolTable* parse_decls(Scope scope);

    bool parse_decl(vector<SymbolDecl*>* decls, Scope scope);

    void parse_decl_start_type(const Token* t, vector<SymbolDecl*>* decls, Scope scope);
    void parse_decl_start_const(const Token* t, vector<SymbolDecl*>* decls, Scope scope);
    void parse_decl_start_struct(const Token* t, vector<SymbolDecl*>* decls, Scope scope);

    SymbolTypeInfo* parse_type_info_start_type(const Token* t);
    SymbolTypeInfo* parse_type_info_start_const(const Token* t);

    void parse_decl_body(SymbolTypeInfo* type, vector<SymbolDecl*>* decls, Scope scope);

    void parse_symbol_decls(SymbolTypeInfo* type, vector<SymbolDecl*>* decls);
    TypedSymbolDecl* parse_symbol_decl(SymbolTypeInfo* type, const Token* id);
    FunDecl* parse_fundecl(SymbolTypeInfo* type, const Token* id, const Token* lp);
    StructDecl* parse_struct_members(const Token* t, const Token* id, const Token* lc);

    SymbolTypeInfo* parse_para_type_info(const Token* start);
    TypedSymbolDecl* parse_para(const Token* start);
    vector<TypedSymbolDecl*>* parse_paras();

    // �ͺ���������صĺ���
    FuncBody* parse_funbody(const Token* lc);
    vector<Statement*>* parse_block_body();
    Statement* parse_stat(bool check = false);
    Statement* parse_if_stat(const Token* keyword);
    Statement* parse_while_stat(const Token* keyword);
    Statement* parse_for_stat(const Token* keyword);
    Statement* parse_do_stat(const Token* keyword);
    
    void update_scope();

    void check_semantic();

    void print_semantic_info()const;

    void pre_gen_code();

    void gen_code();

    void gen_cl_init()const;
    void gen_final_main()const;

    //vector<ArrayDecl*> global_arrays;
    //vector<VarDecl*> global_init_vars;
    SymbolTable global_symbols;
    bool need_init_fun =false;
    vector<FunDecl*> global_functions;

    SymbolTableStack* table_stack;
};


void part6_test();
