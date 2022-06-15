#pragma once
#include "tools.h"
#include "ast.h"
#include "typeinfo.h"


struct SymbolTableStack;
struct TypedSymbolDecl;

//表达式的基类
struct Expr :public ASTNode
{
    SemanticTypeInfo* semantic_info = nullptr;

    Expr()
    {
        semantic_info = new SemanticTypeInfo();
    }

    virtual ~Expr() {

    }

    const string& get_type()const {
        return semantic_info->type_name;
    }

    virtual bool is_lvalue()const {
        return false;
    }

    //语义检查,part4
    virtual void check_semantic(const SymbolTableStack& table_stack) {
        assert(false);
    }

    virtual void print_semantic_info(int level, ostream& os = cout)const {
        ASTNode::print_semantic_info(level, os);
        os << "expression has type ";
        auto info = this->semantic_info;

        os << info->str() << endl;
    }


    //为代码生成做准备
    virtual void pre_gen_code() {
        assert(false);
    }

    //生成对应的汇编代码,part5和part6
    virtual void gen_code(const SymbolTableStack& table_stack)const {
        assert(false);
    }
};

struct TokenExpr :public Expr {
    explicit TokenExpr(const Token* t)
    {
        set_token(t);
    }
    virtual void print(int level, ostream& os = cout)const
    {
        ASTNode::print(level, os);
        os << token->lexme << endl;
    }
};

struct LitExpr :public TokenExpr {
    const Token* lit=nullptr;
    explicit LitExpr(const Token* t)
        :TokenExpr(t)
        ,lit(t)
    {
                        
    }

    virtual void gen_code(const SymbolTableStack& table_stack)const;

    virtual void check_semantic(const SymbolTableStack& table_stack);
};

struct CharExpr :public LitExpr {
    explicit CharExpr(const Token* t)
        :LitExpr(t)
    {
        check_type(t, CHAR_LIT);
    }

    virtual void gen_code(const SymbolTableStack& table_stack)const;
    virtual void check_semantic(const SymbolTableStack& table_stack);
};

struct IntExpr :public LitExpr {
    explicit IntExpr(const Token* t)
        :LitExpr(t)
        {
            check_type(t,INT_LIT);
        }
    virtual void gen_code(const SymbolTableStack& table_stack)const;

    virtual void check_semantic(const SymbolTableStack& table_stack);
};

struct FloatExpr:public LitExpr {
    explicit FloatExpr(const Token* t)
        :LitExpr(t)
    {
        check_type(t, REAL_LIT);
    }
    virtual void gen_code(const SymbolTableStack& table_stack)const;
    virtual void check_semantic(const SymbolTableStack& table_stack);
};

struct StringExpr :public LitExpr {
    explicit StringExpr(const Token* t)
        :LitExpr(t)
    {
        check_type(t, STRING_LIT);
    }
    virtual void gen_code(const SymbolTableStack& table_stack)const;
    virtual void check_semantic(const SymbolTableStack& table_stack);
};


struct IdExpr : public TokenExpr{
    const Token* id = nullptr;
    IdExpr(const Token* id)
        :TokenExpr(id)
        ,id(id)
    {
        check_type(id,ID);
    }

    virtual bool is_lvalue()const {
        return true;
    }

    virtual void gen_code(const SymbolTableStack& table_stack)const;

    virtual void check_semantic(const SymbolTableStack& table_stack);

    //把栈顶的值弹出到变量中
    void gen_store_inst(const SymbolTableStack& table_stack)const;

    const TypedSymbolDecl* get_decl(const SymbolTableStack& table_stack)const;
};

//一元表达式
struct UnaryExpr :Expr {
    const Token* op;
    Expr* expr;
    
    UnaryExpr(const Token* op, Expr* child) :
        op(op),expr(child)
    {
        assert(child != nullptr);
        check_unary_op(op);
        set_token(op);
    }
};

struct PreExpr :public UnaryExpr {
    PreExpr(const Token* op, Expr* child) :
        UnaryExpr(op, child) {

    }
    virtual void print(int level, ostream& os = cout)const
    {
        ASTNode::print(level, os);
        os << "prefix " << op->lexme << endl;
        expr->print(level + 1, os);
    }

    void check_operand(const SymbolTableStack& table_stack);
    void set_seman_info();
    virtual void check_semantic(const SymbolTableStack& table_stack);
};

struct NotExpr :public PreExpr {
    NotExpr(const Token* op, Expr* child) :
        PreExpr(op, child)
    {
        assert(child != nullptr);
        check_type(op, NOT);
    }
    virtual void check_semantic(const SymbolTableStack& table_stack);

    virtual void gen_code(const SymbolTableStack& table_stack)const;
};

struct BitNotExpr :public PreExpr {
    BitNotExpr(const Token* op, Expr* child) :
        PreExpr(op, child)
    {
        assert(child != nullptr);
        check_type(op, BIT_NOT);
    }

    virtual void check_semantic(const SymbolTableStack& table_stack);

    virtual void gen_code(const SymbolTableStack& table_stack)const;
};


struct NegExpr :public PreExpr {
    NegExpr(const Token* op, Expr* child) :
        PreExpr(op,child)
    {
        assert(child != nullptr);
        check_type(op,MINUS);
    }
    virtual void gen_code(const SymbolTableStack& table_stack)const;
};

struct PrefixIncExpr :public PreExpr {
    PrefixIncExpr(const Token* op, Expr* child) :
        PreExpr(op, child)
    {
        assert(child != nullptr);
        check_type(op, INC);
    }
    virtual void check_semantic(const SymbolTableStack& table_stack);

    virtual void gen_code(const SymbolTableStack& table_stack)const;
};

struct PrefixDecExpr :public PreExpr {
    PrefixDecExpr(const Token* op, Expr* child) :
        PreExpr(op, child)
    {
        assert(child != nullptr);
        check_type(op, DEC);
    }
    virtual void check_semantic(const SymbolTableStack& table_stack);

    virtual void gen_code(const SymbolTableStack& table_stack)const;
};

// 后缀运算符,包括后缀++和--
struct PostExpr :public UnaryExpr {
    PostExpr(Expr* child,const Token*op)
        :UnaryExpr(op,child)
    {
        
    }

    virtual void check_semantic(const SymbolTableStack& table_stack);
};

struct PostIncExpr :public PostExpr {
    PostIncExpr(Expr* child, const Token* op)
        :PostExpr(child,op)
    {
        check_type(op,INC);
    }

    virtual void gen_code(const SymbolTableStack& table_stack)const;
};

struct PostDecExpr :public PostExpr {
    PostDecExpr(Expr* child, const Token* op)
        :PostExpr(child, op)
    {
        check_type(op, DEC);
    }

    virtual void gen_code(const SymbolTableStack& table_stack)const;
};

// 中缀二元表达式
struct BinaryExpr :public Expr {
    Expr* first;
    const Token* op;
    Expr* second;
    
    BinaryExpr(Expr* first, const Token* op, Expr* second)
        :
        first(first)
        , op(op)
        , second(second)
    {
        check_bin_op(op);
        set_token(op);
    }

    virtual void print(int level, ostream& os = cout)const
    {
        ASTNode::print(level, os);
        os << "binary " << op->lexme << endl;
        first->print(level + 1, os);
        second->print(level + 1, os);
    }
};

struct ArithExpr :public BinaryExpr {
    ArithExpr(Expr* first, const Token* op, Expr* second)
        :BinaryExpr(first, op, second)
    {

    }

    virtual void check_semantic(const SymbolTableStack& table_stack);

    virtual void gen_code(const SymbolTableStack& table_stack)const;
};

struct AddExpr :public ArithExpr {
    AddExpr(Expr* first, const Token* op, Expr* second)
        :ArithExpr(first, op, second)
    {
        assert(op->match(add_ops));
    }

};

struct MulExpr :public ArithExpr {
    MulExpr(Expr* first, const Token* op, Expr* second)
        :ArithExpr(first, op, second)
    {
        assert(op->match(mul_ops));
    }
};

struct CompExpr :public BinaryExpr {
    CompExpr(Expr* first, const Token* op, Expr* second)
        :BinaryExpr(first, op, second)
    {

    }

    virtual void check_semantic(const SymbolTableStack& table_stack);
    virtual void gen_code(const SymbolTableStack& table_stack)const;
};

struct RelExpr :public CompExpr {
    RelExpr(Expr* first, const Token* op, Expr* second)
        :CompExpr(first, op, second)
    {
        assert(op->match(relation_ops));
    }
};

struct EqExpr :public CompExpr {
    EqExpr(Expr* first, const Token* op, Expr* second)
        :CompExpr(first, op, second)
    {
        assert(op->match(eq_ops));
    }
};

struct BitExpr :public BinaryExpr {
    BitExpr(Expr* first, const Token* op, Expr* second)
        :BinaryExpr(first, op, second)
    {

    }

    virtual void check_semantic(const SymbolTableStack& table_stack);
};

struct BitAndExpr :public BitExpr {
    BitAndExpr(Expr* first, const Token* op, Expr* second)
        :BitExpr(first, op, second)
    {
        assert(op->match(BIT_AND));
    }

    virtual void gen_code(const SymbolTableStack& table_stack)const;
};


struct BitOrExpr :public BitExpr {
    BitOrExpr(Expr* first, const Token* op, Expr* second)
        :BitExpr(first, op, second)
    {
        assert(op->match(BIT_OR));
    }

    virtual void gen_code(const SymbolTableStack& table_stack)const;
};


struct LogicAndExpr :public CompExpr {
    LogicAndExpr(Expr* first, const Token* op, Expr* second)
        :CompExpr(first, op, second)
    {
        assert(op->match(AND));
    }

    //生成对应的汇编代码,part5和part6
    virtual void gen_code(const SymbolTableStack& table_stack)const;
};

struct LogicOrExpr :public CompExpr {
    LogicOrExpr(Expr* first, const Token* op, Expr* second)
        :CompExpr(first, op, second)
    {
        assert(op->match(OR));
    }
    virtual void gen_code(const SymbolTableStack& table_stack)const;
};

struct ElementExpr;

struct AssignExpr :public BinaryExpr {
    AssignExpr(Expr* first, const Token* op, Expr* second)
        :BinaryExpr(first, op, second)
    {
        assert(op->match(assign_ops));
    }

    virtual void check_semantic(const SymbolTableStack& table_stack);
    virtual void gen_code(const SymbolTableStack& table_stack)const;
        
    void gen_op()const;
};

// 类型转换表达式
struct CastExpr :Expr {
    virtual int get_line_no()const {
        return type->pos.line_no;
    }

    virtual void check_semantic(const SymbolTableStack& table_stack);
    virtual void gen_code(const SymbolTableStack& table_stack)const;

    const Token* lp;
    const Token* type;
    const Token* rp;
    Expr* expr;
    CastExpr(const Token* lp, const Token* type, const Token* rp, Expr* expr)
        :
        lp(lp)
        , type(type)
        , rp(rp)
        , expr(expr)
    {
        check_type(lp, LP);
        check_type(type, TYPE);
        check_type(rp, RP);

        set_token(type);
    }
    virtual void print(int level, ostream& os = cout)const
    {
        ASTNode::print(level, os);
        os << "cast " << endl;
        ASTNode::print(level + 1, os);
        os << (type->lexme) << endl;
        expr->print(level + 1, os);
    }

    
};

struct CondExpr :public Expr {
    Expr* cond;
    const Token* qmark;
    Expr* true_branch;
    const Token* colon;
    Expr* false_branch;


    virtual void check_semantic(const SymbolTableStack& table_stack);
    
    virtual void gen_code(const SymbolTableStack& table_stack)const;

    CondExpr(Expr* cond
        , const Token* qmark
        , Expr* true_branch
        , const Token* colon
        , Expr* false_branch)
        :
        cond(cond), qmark(qmark), true_branch(true_branch), colon(colon), false_branch(false_branch) {

        check_type(this->qmark,QN_MARK);
        check_type(this->colon,COLON);
        set_token(qmark);

    }
    virtual void print(int level, ostream& os = cout)const
    {
        ASTNode::print(level, os);
        os << "cond expr " << endl;
        cond->print(level + 1, os);
        true_branch->print(level + 1, os);
        false_branch->print(level + 1, os);
    }
};

//访问数组元素
struct ElementExpr :public Expr {
    Expr* addr;
    const Token* lb;
    Expr* index;
    const Token* rb;

    virtual void check_semantic(const SymbolTableStack& table_stack);
    virtual void gen_code(const SymbolTableStack& table_stack)const;

    void gen_load_inst()const;
    void gen_store_inst()const;

    virtual bool is_lvalue()const {
        return true;
    }

    #if 0
    void inc(ostream& os)const;
    void dec(ostream& os)const;


    //把首地址和索引入栈
    virtual void gen_addr(ostream& os)const;

    #endif

    ElementExpr(Expr* addr, const Token* lb, Expr* index, const Token* rb)
        :
        addr(addr)
        , lb(lb)
        , index(index)
        , rb(rb)
    {
        check_type(lb, LBRACKET);
        check_type(rb, RBRACKET);
        set_token(lb);
    }

    virtual void print(int level, ostream& os = cout)const
    {
        ASTNode::print(level, os);
        os << "element access " << lb->lexme << endl;
        addr->print(level + 1, os);
        index->print(level + 1, os);
    }
};

struct MemberExpr :public Expr {
    Expr* object;
    const Token* op;
    const Token* id;

    virtual bool is_lvalue()const {
        return true;
    }


    virtual void check_semantic(const SymbolTableStack& table_stack);
    
    MemberExpr(Expr* object, const Token* op, const Token* id)
        :
        object(object)
        , op(op)
        , id(id)
    {
        check_type(op, DOT);
        check_type(id,ID);
        set_token(op);
    }

    virtual void print(int level, ostream& os = cout)const
    {
        ASTNode::print(level, os);
        os << "member access : " << (op->lexme) << endl;
        object->print(level + 1, os);
        ASTNode::print(level + 1, os);
        os << (id->lexme) << endl;
    }
};

struct Arguments {
    vector<void*> objs;  // 参数列表

    int argNum()const {
        return (objs.size() + 1) / 2;
    }

    Expr* get(int index)const {
        auto p = objs[2 * index];
        return (Expr*)(p);
    }

    void push(const Token* op) {
        check_type(op, COMMA);
        objs.push_back((void*)op);
    }
    void push(Expr* arg) {
        objs.push_back(arg);
    }

};

struct CallExpr :public Expr {
    const Token* fun_name;
    const Token* lp;
    Arguments* args;
    const Token* rp;

    virtual void check_semantic(const SymbolTableStack& table_stack);
    virtual void gen_code(const SymbolTableStack& table_stack)const;

    CallExpr(const Token* fun_name, const Token* lp, Arguments* args, const Token* rp)
        :
        fun_name(fun_name)
        , lp(lp)
        , args(args)
        , rp(rp)
    {
        check_type(this->fun_name,ID);
        check_type(this->lp, LP);
        check_type(this->rp, RP);
        set_token(fun_name);
    }

    virtual void print(int level, ostream& os = cout)const {
        ASTNode::print(level, os);
        os << "call expr:" << endl;
        ASTNode::print(level + 1, os);
        os << "funname:" <<fun_name;
        //first->print(0, os);
        ASTNode::print(level + 1, os);
        int n = args->argNum();
        os << n << " Args:" << endl;
        for (int i = 0;i < n;++i) {
            ASTNode::print(level + 1, os);
            os << "args[" << i << "]:" << endl;
            args->get(i)->print(level + 2, os);
        }
    }
};
