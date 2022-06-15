#pragma once
#include "tools.h"
#include "ast.h"
#include "expr.h"
#include "statement.h"
#include "scope.h"
#include "symbol_table.h"


// 每个声明之前的类型信息,语法分析时得到
struct SymbolTypeInfo {
    const Token* const_token = nullptr;  //声明时是否有const关键字
    const Token* struct_token = nullptr;   //必须是TYPE或者STRUCT
    const Token* type = nullptr;  //可以是变量名,数组名,函数名,结构体名

    SymbolTypeInfo(const Token* const_token,const Token* struct_token, const Token* type) :
        const_token(const_token),struct_token(struct_token),type(type){
        assert(this->type!=nullptr);
        if (this->const_token!=nullptr)
            check_type(const_token,CONST);
        if (this->struct_token != nullptr) {
            check_type(this->struct_token,STRUCT);
            check_type(this->type,ID);
        }
        else {
            check_type(this->type, TYPE);
        }
    }
};


struct SymbolDecl:ASTNode{
    const Token* name = nullptr;  //可以是变量名,数组名,函数名,结构体名

    string utf_name = "";    //变量,数组和函数的签名.用于代码生成

    SymbolDecl(const Token* name)
        :
        name(name) 
    {
        check_type(name,ID);
        set_token(name);
    }

    Scope scope; //记录本对象的作用域,输出语义信息时会用到
    
    virtual void set_scope(Scope scope){
        this->scope =scope;
    }

    //语义检查,part4
    virtual void check_semantic(SymbolTableStack& table_stack) {
        show_semantic_error(this->token, "not implemented");
    }

    //为代码生成做准备
    virtual void pre_gen_code() {
        show_semantic_error(this->token, "not implemented");
    }
    
};

// 有类型的对象,包括,变量,数组,函数
struct TypedSymbolDecl :public SymbolDecl {
    const SymbolTypeInfo* type_tokens = nullptr;
    const Token* const_token = nullptr;  //是否有const限制

    SemanticTypeInfo* semantic_info = nullptr;

    
    int symbol_index = -1; //在栈中的下标,用于局部变量

    TypedSymbolDecl(const SymbolTypeInfo* type_info, const Token* name)
        :
        SymbolDecl(name)
        ,type_tokens(type_info)
    {
        semantic_info =new SemanticTypeInfo();
    }

    
    void set_seman_info(SymbolTableStack& table_stack);
    
};

struct VarDecl :public TypedSymbolDecl {
    const Token* assign = nullptr;
    Expr* init_expr = nullptr;

    //为代码生成做准备,设置全局变量的utf_name
    virtual void pre_gen_code();
    void gen_init_code(const SymbolTableStack& table_stack)const;

    
    virtual void check_semantic(SymbolTableStack& table_stack);
    //检查初始化的表达式是否类型合法
    void check_init(const SymbolTableStack& table_stack);


    VarDecl(const SymbolTypeInfo* type_info, const Token* name)
        :TypedSymbolDecl(type_info, name)
    {

    }
    VarDecl(const SymbolTypeInfo* type_info, const Token* name
        , const Token* assign
        , Expr* expr
    )
        :TypedSymbolDecl(type_info, name)
        , assign(assign)
        , init_expr(expr)
    {
        check_type(this->assign, ASSGIN);
        assert(expr != nullptr);
    }

    virtual void print_semantic_info(int level, ostream& os = cout) const;
};

struct ArrayDecl :public TypedSymbolDecl {
    const Token* lb;
    const Token* size = nullptr;  //对函数参数,这个成员可以为空
    const Token* rb;

    virtual void pre_gen_code();
    void gen_init_code()const;

    ArrayDecl(const SymbolTypeInfo* type_info, const Token* name, const Token* lb,
        const Token* size,
        const Token* rb)
        :TypedSymbolDecl(type_info, name)
        , lb(lb)
        , size(size)
        , rb(rb)
    {
        check_type(this->lb, LBRACKET);
        check_type(this->rb, RBRACKET);
        if (size != nullptr)
            check_type(this->size, INT_LIT);
    }

    virtual void check_semantic(SymbolTableStack& table_stack);
    virtual void print_semantic_info(int level, ostream& os = cout) const;
};

struct FuncBody :ASTNode {
    const Token* lbrace;
    vector<SymbolDecl*>* decls;
    vector<Statement*>* stats;
    vector<ASTNode*>* objs;   //按行号存储所有对象(包括声明和语句)
    const Token* rbrace;


    //生成对应的汇编代码,part5和part6
    bool gen_code(const SymbolTableStack& table_stack)const; 
    
    virtual void check_semantic(SymbolTableStack& table_stack);
    virtual void print_semantic_info(int level, ostream& os = cout)const;


    void set_scope() {
        for (auto e : *decls) {
            e->set_scope(LOCAL_SCOPE);
        }
    }

    FuncBody(const Token* lbrace, vector<SymbolDecl*>* decls, vector<Statement*>* stats
        , vector<ASTNode*>* objs
        , const Token* rbrace)
        :
        lbrace(lbrace)
        ,decls(decls)
        ,stats(stats)
        , objs(objs)
        , rbrace(rbrace)
    {
        check_type(this->lbrace, LBRACE);
        check_type(this->rbrace, RBRACE);
        assert(this->decls!=nullptr);
        assert(this->stats != nullptr);
        assert(this->objs != nullptr);
    }

    virtual void print(int level, ostream& os = cout)const {
        ASTNode::print(level, os);
        os << "body info:" << endl;

        int n = decls->size();
        ASTNode::print(level + 1, os);
        os << n << " variables" << endl;
        for (int i = 0;i < n;++i) {
            ASTNode::print(level + 2, os);
            os << "local variable " << i << endl;
            (*decls)[i]->print(level + 3, os);
        }
    }
};

struct FunDecl: public TypedSymbolDecl {
    const Token* lp = nullptr;
    vector<TypedSymbolDecl*>* paras = nullptr;
    const Token* rp = nullptr;

    const Token* semicolon = nullptr;
    FuncBody* body = nullptr;

    int var_num; //函数和局部变量的总个数

    //代码生成之前的准备工作
    virtual void pre_gen_code();

    SymbolTable all_vars;  //存储所有参数和局部变量的符号表,用于代码生成.
    void gen_code(const SymbolTableStack& table_stack)const;
    void check_return(bool return_flag)const;
    void gen_comment_for_vars()const;

    // 语义检查用到的函数和成员
    SymbolTable table; //语义检查时用到的表
    virtual void check_semantic(SymbolTableStack& table_stack);
    virtual void print_semantic_info(int level, ostream& os = cout)const;
    string proto_str()const;

    //判断两个函数的声明是否完全一致
    bool check_same_proto(const FunDecl* other)const;
    bool check_same_paras(const FunDecl* other)const;


    virtual void set_scope(Scope scope) {
        assert(scope==GLOBAL_SCOPE);
        SymbolDecl::set_scope(scope);
        for (auto e : *paras) {
            e->set_scope(PARA_SCOPE);
        }
        if (body != nullptr) {
            body->set_scope();
        }
    }

    FunDecl(const SymbolTypeInfo* type_info, const Token* name, const Token* lp, vector<TypedSymbolDecl*>* paras
        , const Token* rp, const Token* semicolon)
        :TypedSymbolDecl(type_info, name)
        , lp(lp)
        , paras(paras)
        , rp(rp)
        , semicolon(semicolon)
    {
        check_type(this->lp, LP);
        check_type(this->rp, RP);
    }
    FunDecl(const SymbolTypeInfo* type_info, const Token* name, const Token* lp, vector<TypedSymbolDecl*>* paras
        , const Token* rp, FuncBody* body)
        : TypedSymbolDecl(type_info, name)
        , lp(lp)
        , paras(paras)
        , rp(rp)
        , body(body)
    {
        check_type(this->lp, LP);
        check_type(this->rp, RP);
        assert(this->body != nullptr);
    }

};



struct StructDecl :public SymbolDecl {
    const Token* struct_token = nullptr;
    const Token* id = nullptr;   //自定义结构名
    const Token* lc = nullptr;
    SymbolTable* members = nullptr;   //成员列表
    const Token* rc = nullptr;
    const Token* semi = nullptr;


    SymbolTable member_table; //语义检查时构造

    virtual void check_semantic(SymbolTableStack& table_stack);
    virtual void print_semantic_info(int level, ostream& os = cout) const;

    virtual void set_scope(Scope scope) {
        SymbolDecl::set_scope(scope);
        for (auto e : *members) {
            e->set_scope(STRUCT_SCOPE);
        }
    }

    #if 0
    VarDecl* operator[](int index)const {
        return dynamic_cast<VarDecl*>((*members)[index]);
    }

    int size()const {
        return members->size();
    }
    #endif

    const TypedSymbolDecl* find_member(const string& name)const;

    StructDecl(const Token* struct_token
        , const Token* id
        , const Token* lc
        , SymbolTable* members
        , const Token* rc
        , const Token* semi
    )
        :
        SymbolDecl(id)
        , struct_token(struct_token)
        , id(id)
        , lc(lc)
        , members(members)
        , rc(rc)
        , semi(semi)
    {
        check_type(this->struct_token, STRUCT);
        check_type(this->id, ID);
        check_type(this->lc, LBRACE);
        check_type(this->rc, RBRACE);
        check_type(this->semi, SEMICOLON);

        assert(this->members != nullptr);
    }
};
