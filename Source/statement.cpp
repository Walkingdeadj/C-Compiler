#include "decl.h"
#include "statement.h"
#include "table_stack.h"
#include "semantic_error.h"
#include "cginfo.h"

void ReturnStatement::gen_code(const SymbolTableStack& table_stack) const
{
    assert(table_stack.current_fun != nullptr);
    auto fun_info = table_stack.current_fun->semantic_info;
    auto type = fun_info->type_name;
    assert(fun_info->type == TYPE);

    auto& os =*jout;
#if 1
    print_tabs(os);
    os << ";; return " << semi->get_filename() << " "
        << semi->get_line_no() << endl;
#endif

    if (this->expr != nullptr) {
        //将返回类型转换为需要的类型
        expr->gen_code(table_stack);
        auto expr_info = expr->semantic_info;
        auto expr_type = expr_info->type_name;
        if (expr_type != type) {
            gen_convert_inst(os, type, expr_type);
        }
        print_tabs(os);
        if (type == "int"||type=="char")
            os << "ireturn" << endl;
        else if (type == "float")
            os << "freturn" << endl;
    }
    else {
        print_tabs(os);
        os << "return" << endl;
    }
}

void ReturnStatement::check_semantic(const SymbolTableStack& table_stack)
{
    assert(table_stack.current_fun != nullptr);

    auto fun_info = (table_stack.current_fun)->semantic_info;
    if (this->expr != nullptr) {
        this->expr->check_semantic(table_stack);
        auto expr_info = expr->semantic_info;
        if (fun_info->type_name == "void" && expr_info->type_name != "void") {
            ostringstream oss;
            oss << "Return type mismatch: was " << expr_info->str() << ", expected void";
            show_semantic_error(semi, oss);
        }
        else {
            if (!expr_info->is_compatible_with_para(fun_info)) {
                ostringstream oss;
                oss << "Return type mismatch: was " << expr_info->str() << ", expected " << fun_info->str();
                show_semantic_error(semi, oss);
            }
        }
    }
    else {
        if (fun_info->type_name != "void") {
            ostringstream oss;
            oss << "Return type mismatch: was void, expected " << fun_info->str();
            show_semantic_error(semi, oss);
        }
    }
}

void ExprStatement::gen_code(const SymbolTableStack& table_stack) const
{
    auto& os =*jout;
    print_tabs(os);
    os << ";; expression " << semi->get_filename() << " "
        << semi->get_line_no() << endl;
    expr->gen_code(table_stack);


    if (expr->semantic_info->type_name!="void")
        print_inst("pop");
}

void ExprStatement::check_semantic(const SymbolTableStack& table_stack)
{
    expr->check_semantic(table_stack);
}

void BreakStatement::gen_code(const SymbolTableStack& table_stack) const
{
    if (cginfo->loop_stack.empty()) {
        show_cg_error(token,"break statement not within a loop");
    }
    else {
        auto s = cginfo->loop_stack.back();
        assert(s != nullptr);
        assert(s->after_label != 0);
        print_goto(s->after_label);
    }
}

void IfStatement::gen_code(const SymbolTableStack& table_stack) const
{
    auto& os = *jout;
    if (false_body == nullptr) {
        print_comment("following is if condtion");
        cond->gen_code(table_stack);
        int L2 = cginfo->new_label();
        
        print_tabs(os);
        os << "ifeq L" << L2 << endl;

        true_body->gen_code(table_stack);
        print_label(L2);
    }
    else{
        int L1 = cginfo->new_label();
        int L2 = cginfo->new_label();
        
        print_comment("following is if condtion");
        cond->gen_code(table_stack);
        print_tabs(os);
        os << "ifeq L" << L1 << endl;
        
        true_body->gen_code(table_stack);
        print_goto(L2);
        
        print_label(L1);
        false_body->gen_code(table_stack);
        print_label(L2);
    }
}

void IfStatement::check_semantic(const SymbolTableStack& table_stack)
{
    this->cond->check_semantic(table_stack);
    auto info = this->cond->semantic_info;
    if (!info->compatible_with_float()) {
        ostringstream oss;
        oss << "if condition has non-numeric type " <<info->type_name;
        show_semantic_error(token,oss);
    }
    true_body->check_semantic(table_stack);
    if (false_body!=nullptr)
        false_body->check_semantic(table_stack);
}

void DoStatement::gen_code(const SymbolTableStack& table_stack) const
{
    int L1 = cginfo->new_label();
    int L2 = cginfo->new_label();
    continue_target = L1;
    after_label = L2;

    cginfo->loop_stack.push_back(this);

    print_label(L1);
    true_body->gen_code(table_stack);
    cond->gen_code(table_stack);

    auto& os = *jout;
    print_tabs(os);
    os << "ifne L" << L1 << endl;
    print_label(L2);

    cginfo->loop_stack.pop_back();
}

void DoStatement::check_semantic(const SymbolTableStack& table_stack)
{
    true_body->check_semantic(table_stack);
    this->cond->check_semantic(table_stack);
    auto info = this->cond->semantic_info;
    if (!info->compatible_with_float()) {
        ostringstream oss;
        oss << "Dowhile condition has non-numeric type " << info->type_name;
        show_semantic_error(cond->token, oss);
    }
}

void ForStatement::gen_code(const SymbolTableStack& table_stack) const
{
    auto& os = *jout;
    int L1 = cginfo->new_label();
    int L2 = cginfo->new_label();
    int L3 = cginfo->new_label();

    continue_target = L2;
    after_label = L3;
    
    {
        ostringstream oss;
        oss << "code generation of  for loop begin at " <<token->get_line_no();
        print_comment(oss.str());
    }
    
    if (expr1!=nullptr){
        print_comment("init expression of for loop");
        expr1->gen_code(table_stack);

        if (expr1->semantic_info->type_name != "void")
            print_inst("pop");
    }
    else {
        print_comment("initial expression of for loop is null");
    }
    print_label(L1);


    if (cond!=nullptr){
        print_comment("cond expression of for loop");
        cond->gen_code(table_stack);
        print_tabs(os);
        os << "ifeq L" << L3 << endl;
    }
    else {
        print_comment("cond expression of for loop is null");
    }

    
    cginfo->loop_stack.push_back(this);

    print_comment("following is body of for loop");
    true_body->gen_code(table_stack);
    cginfo->loop_stack.pop_back();

    print_comment("after body of for loop");
    print_label(L2);
    if (expr3!=nullptr){
        print_comment("end expression of for loop");
        expr3->gen_code(table_stack);
        if (expr3->semantic_info->type_name != "void")
            print_inst("pop");
    }
    else {
        print_comment("end expression of for loop is null");
    }
    {
        ostringstream oss;
        oss << "go back to start of for loop begin at " << token->get_line_no();
        print_comment(oss.str());
    }
    print_goto(L1);

    {
        ostringstream oss;
        oss << "outer of for loop begin at " << token->get_line_no();
        print_comment(oss.str());
    }
    print_label(L3);
}

void ForStatement::check_semantic(const SymbolTableStack& table_stack)
{
    if (expr1!=nullptr)
        expr1->check_semantic(table_stack);
    if (cond!= nullptr) {
        this->cond->check_semantic(table_stack);
        auto info = this->cond->semantic_info;
        if (!info->compatible_with_float()) {
            ostringstream oss;
            oss << "For condition has non-numeric type " << info->type_name;
            show_semantic_error(token, oss);
        }
    }
    if (expr3!=nullptr)
        expr3->check_semantic(table_stack);

    true_body->check_semantic(table_stack);
}

void WhileStatement::gen_code(const SymbolTableStack& table_stack) const
{
    
    auto& os = *jout;
    int L1 = cginfo->new_label();
    int L2 = cginfo->new_label();
    continue_target =L1;
    after_label = L2;

    cginfo->loop_stack.push_back(this);

    print_comment("While condition follows");
    print_label(L1);
    cond->gen_code(table_stack);

    print_comment("if false jump");
    print_tabs(os);
    os << "ifeq L" << L2 << endl;
    true_body->gen_code(table_stack);
    print_goto(L1);
    print_label(L2);

    cginfo->loop_stack.pop_back();
}

void WhileStatement::check_semantic(const SymbolTableStack& table_stack)
{
    this->cond->check_semantic(table_stack);
    auto info = this->cond->semantic_info;
    if (!info->compatible_with_float()) {
        ostringstream oss;
        oss << "While condition has non-numeric type " << info->type_name;
        show_semantic_error(token, oss);
    }
    true_body->check_semantic(table_stack);
}

void ContinueStatement::gen_code(const SymbolTableStack& table_stack) const
{
    if (cginfo->loop_stack.empty()) {
        show_cg_error(token, "continue statement not within a loop");
    }
    else {
        auto s = cginfo->loop_stack.back();
        assert(s!=nullptr);
        assert(s->continue_target!=0);
        print_goto(s->continue_target);
    }
}

void BlockStatement::gen_code(const SymbolTableStack& table_stack) const
{
    for (auto e : *body) {
        e->gen_code(table_stack);
   }
}

void BlockStatement::check_semantic(const SymbolTableStack& table_stack)
{
    for (auto e : *body) {
        e->check_semantic(table_stack);
    }
}
