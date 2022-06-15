#pragma once
#include "tools.h"
#include "token.h"
#include "semantic_error.h"
#include "ast.h"
#include "expr.h"

struct SymbolTableStack;
struct Statement :public ASTNode
{
    
    Statement()
    {
        
    }

    virtual ~Statement() {
        
    }

    virtual void check_semantic(const SymbolTableStack& table_stack) {
       
    }

    virtual void print_semantic_info(int level, ostream& os = cout)const
    {

    }

    //为代码生成做准备
    virtual void pre_gen_code() {
        
    }

    //生成对应的汇编代码,part5和part6
    virtual void gen_code(const SymbolTableStack& table_stack)const {
        print_inst("nop");
    }
};

struct SemiStatement :public Statement {
    const Token* semi = nullptr;
    virtual void print_semantic_info(int level, ostream& os = cout)const
    {
        
    }
    SemiStatement(const Token* semi)
        :
        semi(semi)
    {
        check_type(this->semi, SEMICOLON);
        set_token(semi);
    }
    virtual void print(int level, ostream& os = cout)const {
        ASTNode::print(level, os);
        os << "empty statement " << endl;
    }
};

struct ReturnStatement :public SemiStatement {
    const Token* return_token = nullptr;
    Expr* expr = nullptr;


    virtual void gen_code(const SymbolTableStack& table_stack)const;

    virtual void check_semantic(const SymbolTableStack& table_stack);

    explicit ReturnStatement(const Token* return_token, Expr* expr, const Token* semi)
        :
        SemiStatement(semi)
        , return_token(return_token)
        , expr(expr)
    {
        check_type(this->return_token, RETURN);
    }
    virtual void print(int level, ostream& os = cout)const {
        ASTNode::print(level, os);
        os << "return ";
        if (expr == nullptr) {
            os << " nothing";
        }
        else {
            os << " expression";
        }
        os << endl;

        if (expr != nullptr)
            expr->print(level + 1, os);
    }
};

struct BreakStatement :public SemiStatement {
    const Token* break_token = nullptr;

    BreakStatement(const Token* break_token, const Token* semi) :
        SemiStatement(semi)
        ,break_token(break_token)
    {
        check_type(this->break_token, BREAK);
    }
    virtual void print(int level, ostream& os = cout)const {
        ASTNode::print(level, os);
        os << "break statement " << endl;
    }

    //生成对应的汇编代码,part5和part6
    virtual void gen_code(const SymbolTableStack& table_stack)const;
};

struct ContinueStatement :public SemiStatement {
    const Token* continue_token = nullptr;

    ContinueStatement(const Token* continue_token, const Token* semi) :
        SemiStatement(semi)
        , continue_token(continue_token)
    {
        check_type(this->continue_token, CONTINUE);
    }
    virtual void print(int level, ostream& os = cout)const {
        ASTNode::print(level, os);
        os << "break statement " << endl;
    }

    //生成对应的汇编代码,part5和part6
    virtual void gen_code(const SymbolTableStack& table_stack)const;
};


struct ExprStatement :public SemiStatement {
    Expr* expr = nullptr;

    virtual void gen_code(const SymbolTableStack& table_stack)const;
    
    virtual void check_semantic(const SymbolTableStack& table_stack);

    virtual void print_semantic_info(int level, ostream& os = cout)const
    {
        expr->print_semantic_info(1, os);
    }
    
    explicit ExprStatement(Expr* expr, const Token* semi)
        :
        SemiStatement(semi)
        , expr(expr)
    {
        assert(this->expr != nullptr);
    }
    virtual void print(int level, ostream& os = cout)const {
        ASTNode::print(level, os);
        os << "expression statement " << endl;
        expr->print(level + 1, os);
    }
};

struct BlockStatement :public Statement {
    const Token* lc = nullptr;
    vector<Statement*>* body = nullptr;
    const Token* rc = nullptr;

    //生成对应的汇编代码,part5和part6
    virtual void gen_code(const SymbolTableStack& table_stack)const;
    virtual void check_semantic(const SymbolTableStack& table_stack);

    virtual void print_semantic_info(int level, ostream& os = cout)const {
        for (auto const e : *body)
            e->print_semantic_info(level, os);
    }

    BlockStatement(const Token* lc, vector<Statement*>* body, const Token* rc)
        :lc(lc), body(body), rc(rc)
    {
        check_type(this->lc, LBRACE);
        check_type(this->rc, RBRACE);
        assert(this->body != nullptr);
        set_token(lc);
    }
    virtual void print(int level, ostream& os = cout)const {
        ASTNode::print(level, os);
        int n = body->size();
        os << "block statement contains " << n << " statemens" << endl;
        for (int i = 0;i < n;++i) {
            ASTNode::print(level + 1, os);
            os << "statement " << i << endl;
            (*body)[i]->print(level + 2, os);
        }
    }
};

//带条件的语句
struct CondStatement :public Statement {
    mutable int continue_target = 0;
    mutable int after_label = 0;

    const Token* lp = nullptr;
    Expr* cond = nullptr;  //对for语句,可能为空
    const Token* rp = nullptr;
    Statement* true_body;

    virtual void print_semantic_info(int level, ostream& os = cout)const {
        true_body->print_semantic_info(level, os);
    }

    CondStatement(const Token* lp, Expr* cond, const Token* rp, Statement* true_body)
        :lp(lp), cond(cond), rp(rp), true_body(true_body)
    {
        check_type(this->lp, LP);

        check_type(this->rp, RP);
        assert(this->true_body != nullptr);
        set_token(lp);
    }
};

struct IfStatement :public CondStatement {
    const Token* if_token = nullptr;
    const Token* else_token = nullptr;
    Statement* false_body = nullptr;


    virtual void gen_code(const SymbolTableStack& table_stack)const;
    virtual void check_semantic(const SymbolTableStack& table_stack);

    virtual void print_semantic_info(int level, ostream& os = cout)const {
        CondStatement::print_semantic_info(level, os);
        if (false_body != nullptr)
            false_body->print_semantic_info(level, os);
    }

    IfStatement(
        const Token* if_token,
        const Token* lp,
        Expr* cond,
        const Token* rp,
        Statement* true_branch
    )
        :CondStatement(lp, cond, rp, true_branch)
        ,if_token(if_token)
    {
        check_type(this->if_token, IF);
        assert(this->cond != nullptr);
        set_token(if_token);
    }

    IfStatement(
        const Token* if_token,
        const Token* lp,
        Expr* cond,
        const Token* rp,
        Statement* true_branch,
        const Token* else_token,
        Statement* false_branch
    )
        :
        CondStatement(lp, cond, rp, true_branch)
        ,if_token(if_token)
        , else_token(else_token)
        , false_body(false_branch)
    {
        check_type(this->if_token, IF);
        assert(this->cond != nullptr);

        check_type(this->else_token, ELSE);
        assert(this->false_body != nullptr);

        set_token(if_token);
    }

    virtual void print(int level, ostream& os = cout)const {
        ASTNode::print(level, os);
        os << "if statement" << endl;

        ASTNode::print(level + 1, os);
        os << "condtion:" << endl;
        cond->print(level + 2, os);

        ASTNode::print(level + 1, os);
        os << "true body:" << endl;
        true_body->print(level + 2, os);

        if (false_body != nullptr) {
            ASTNode::print(level + 1, os);
            os << "else body:" << endl;
            false_body->print(level + 2, os);
        }
    }
};

struct WhileStatement :public CondStatement {
    virtual void gen_code(const SymbolTableStack& table_stack)const;
    virtual void check_semantic(const SymbolTableStack& table_stack);

    const Token* while_token;
    WhileStatement(const Token* while_token, const Token* lp, Expr* cond,
        const Token* rp,
        Statement* body)
        :
        CondStatement(lp, cond, rp, body)
        ,while_token(while_token)
    {
        check_type(this->while_token, WHILE);
        assert(this->cond != nullptr);

        set_token(while_token);
    }

    virtual void print(int level, ostream& os = cout)const {
        ASTNode::print(level, os);
        os << "while loop" << endl;

        ASTNode::print(level + 1, os);
        os << "condtion:" << endl;
        cond->print(level + 2, os);

        ASTNode::print(level + 1, os);
        os << "loop body:" << endl;
        true_body->print(level + 2, os);
    }
};

struct DoStatement :public WhileStatement {
    virtual void gen_code(const SymbolTableStack& table_stack)const;
    virtual void check_semantic(const SymbolTableStack& table_stack);

    const Token* do_token;
    const Token* semi;
    

    DoStatement(const Token* do_token, Statement* body, const Token* while_token, const Token* lp,
        Expr* cond, const Token* rp, const Token* semi)
        :WhileStatement(while_token, lp, cond, rp, body)
        , do_token(do_token)
        , semi(semi)
    {
        check_type(this->do_token, DO);
        check_type(this->while_token, WHILE);
        set_token(do_token);

        assert(this->cond!=nullptr);
    }

    virtual void print(int level, ostream& os = cout)const {
        ASTNode::print(level, os);
        os << "do-while loop" << endl;

        ASTNode::print(level + 1, os);
        os << "loop body:" << endl;
        true_body->print(level + 2, os);

        ASTNode::print(level + 1, os);
        os << "condtion:" << endl;
        cond->print(level + 2, os);
    }
};

struct ForStatement :public CondStatement {
    virtual void gen_code(const SymbolTableStack& table_stack)const;
    virtual void check_semantic(const SymbolTableStack& table_stack);

    const Token* for_token =nullptr;
    Expr* expr1 = nullptr;
    const Token* semi1;
    const Token* semi2;
    Expr* expr3 = nullptr;


    ForStatement(const Token* for_token, const Token* lp, Expr* expr1
        , const Token* semi1
        , Expr* expr2
        , const Token* semi2
        , Expr* expr3
        , const Token* rp
        , Statement* body)

        :CondStatement(lp, expr2, rp, body)
        ,for_token(for_token)
        , expr1(expr1)
        , semi1(semi1)
        , semi2(semi2)
        , expr3(expr3)
    {
        check_type(this->for_token, FOR);
    }

    virtual void print(int level, ostream& os = cout)const {
        ASTNode::print(level, os);
        os << "for loop" << endl;

        ASTNode::print(level + 1, os);
        os << "init part: " << (expr1 == nullptr ? "None" : "") << endl;
        if (expr1 != nullptr)
            expr1->print(level + 2, os);

        ASTNode::print(level + 1, os);
        os << "condtion part: " << (cond == nullptr ? "None" : "") << endl;
        if (cond != nullptr)
            cond->print(level + 2, os);

        ASTNode::print(level + 1, os);
        os << "end expression of loop : " << (expr3 == nullptr ? "None" : "") << endl;
        if (expr3 != nullptr)
            expr3->print(level + 2, os);
        ASTNode::print(level + 1, os);
        os << "loop body:" << endl;
        true_body->print(level + 2, os);
    }
};

