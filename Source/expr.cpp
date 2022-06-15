#include "expr.h"
#include "decl.h"
#include "table_stack.h"
#include "semantic_error.h"
#include "cginfo.h"

void LitExpr::gen_code(const SymbolTableStack& table_stack) const
{
    assert(false);
}

void LitExpr::check_semantic(const SymbolTableStack& table_stack)
{
    auto info = this->semantic_info;
    info->is_const = true;
    info->is_array = false;
    this->semantic_info->type = TYPE;
}

void CharExpr::gen_code(const SymbolTableStack& table_stack) const
{
    auto& os = *jout;
    print_tabs(os);
    os << "; char constant" << endl;
    const string& s = token->lexme;
    char c = s[1];
    if (c == '\\')
    {
        switch (s[2])
        {
        case 't':
            c = '\t';
            break;
        case 'r':
            c = '\r';
            break;
        case 'n':
            c = '\n';
            break;
        case 'a':
            c = '\a';
            break;
        case 'b':
            c = '\b';
            break;
        default:
            assert(false);
            break;
        }
    }
    print_tabs(os);
    os << "bipush " << ((int)c) << endl;
}

void CharExpr::check_semantic(const SymbolTableStack& table_stack)
{
    LitExpr::check_semantic(table_stack);
    this->semantic_info->type_name = "char";
}

void IntExpr::gen_code(const SymbolTableStack& table_stack) const
{
    int value = my_atoi(token->lexme);

    if (value >= 0 && value <= 5) {
        ostringstream oss;
        oss << "iconst_" << value;
        print_inst(oss.str());
    }

    else {
        auto& os = *jout;
        print_tabs(os);
        os << "ldc " << value << endl;
    }
}

void IntExpr::check_semantic(const SymbolTableStack& table_stack)
{
    LitExpr::check_semantic(table_stack);
    this->semantic_info->type_name = "int";
}

void FloatExpr::gen_code(const SymbolTableStack& table_stack) const
{
    auto value=atof(token->lexme.c_str());
    if (value == 0) {
        print_inst("fconst_0");
    }
    else if (value == 1) {
        print_inst("fconst_1");
    }
    else if (value == 2) {
        print_inst("fconst_2");
    }
    else{
        auto& os = *jout;
        print_tabs(os);
        os << "ldc +" <<fixed<<setprecision(6) << value << "f" << endl;
    }
}

void FloatExpr::check_semantic(const SymbolTableStack& table_stack)
{
    LitExpr::check_semantic(table_stack);
    this->semantic_info->type_name = "float";
}

void IdExpr::gen_code(const SymbolTableStack& table_stack) const
{
    auto name = token->lexme;
    auto decl = table_stack.find_id(name);
    assert(decl != nullptr);
    auto info = decl->semantic_info;
    //assert(!info->is_array);
    auto scope = decl->scope;
    auto& os = *jout;
    print_tabs(os);

    if (scope == GLOBAL_SCOPE) {
        os << "getstatic Field " << casename << " " << name << " " << decl->utf_name << endl;
        return;
    }
    if (!info->is_array) {
        if (scope == LOCAL_SCOPE || scope == PARA_SCOPE) {
            auto type = info->type_name;
            if (type == "float")
                os << "fload";
            else
                os << "iload";
            assert(decl->symbol_index >= 0);
            int index = decl->symbol_index;
            assert(index>=0);
            if (index < 4)
                os << "_";
            else
                os << " ";
            os << decl->symbol_index;
            os << " ; load from " << name;
            os << endl;
        }
    }
    else {
        // 把局部数组的起始地址入栈
        if (scope == LOCAL_SCOPE || scope == PARA_SCOPE) {
            os << "aload";
            assert(decl->symbol_index >= 0);
            if (decl->symbol_index<4)
                os << "_";
            else
                os << " ";
            os << decl->symbol_index << endl;
        }
    }
}

void IdExpr::check_semantic(const SymbolTableStack& table_stack)
{
    auto name = token->lexme;
    const TypedSymbolDecl* decl = table_stack.find_id(name);
    if (decl == nullptr)
    {
        ostringstream oss;
        oss << "Unknown identifier '" << name << "'";
        show_semantic_error(token, oss);
    }
    else
        this->semantic_info = decl->semantic_info;
}

void IdExpr::gen_store_inst(const SymbolTableStack& table_stack) const
{
    auto name = token->lexme;
    auto decl = table_stack.find_id(name);
    assert(decl != nullptr);
    auto info = decl->semantic_info;

    auto& os = *jout;
    assert(!info->is_array);
    print_tabs(os);
    auto type = info->type_name;
    auto scope = decl->scope;
    if (scope == LOCAL_SCOPE || scope == PARA_SCOPE) {
        if (type == "float") {
            os << "fstore";
        }
        else {
            os << "istore";
        }

        assert(decl->symbol_index >= 0);
        if (decl->symbol_index<4)
            os << "_";
        else
            os << " " ;
        os<< decl->symbol_index;
        os << endl;
    }
    else if (scope == GLOBAL_SCOPE) {
        print_tabs(os);
        os << "putstatic Field " << casename << " " << name << " " << decl->utf_name << endl;
    }
}

const TypedSymbolDecl* IdExpr::get_decl(const SymbolTableStack& table_stack) const
{
    auto name = token->lexme;
    auto decl = table_stack.find_id(name);
    return decl;
}


void StringExpr::gen_code(const SymbolTableStack& table_stack) const
{
    auto& os =*jout;
    auto n = token->lexme.size();
    string s = token->lexme.substr(1, n - 2);
    s += "\\x00";
    os << "ldc \'" << s << '\'' << endl;

    print_tabs(os);
    os << "invokevirtual Method java/lang/String toCharArray ()[C" << endl;
}

void StringExpr::check_semantic(const SymbolTableStack& table_stack)
{
    LitExpr::check_semantic(table_stack);
    this->semantic_info->type_name = "char";
    this->semantic_info->is_array = true;
}

void PreExpr::check_operand(const SymbolTableStack& table_stack)
{
    expr->check_semantic(table_stack);
    auto expr_info = expr->semantic_info;
    if (!(expr_info->compatible_with_float())) {
        ostringstream oss;
        oss << "No match for unary operation " << op->lexme << " " << expr_info->str();
        show_semantic_error(op, oss);
    }
}

void PreExpr::set_seman_info()
{
    auto expr_info = expr->semantic_info;
    auto res_info = this->semantic_info;
    res_info->is_array = false;
    res_info->type = TYPE;
    res_info->is_const = expr_info->is_const;
    res_info->type_name = expr_info->type_name;
}

void PreExpr::check_semantic(const SymbolTableStack& table_stack)
{
    check_operand(table_stack);
    set_seman_info();
}

void NotExpr::check_semantic(const SymbolTableStack& table_stack)
{
    PreExpr::check_semantic(table_stack);
    auto res_info = this->semantic_info;
    res_info->type_name = "char";
}

void NotExpr::gen_code(const SymbolTableStack& table_stack) const
{
    auto e = dynamic_cast<NotExpr*> (expr);
    if (e != nullptr) {
        e->expr->gen_code(table_stack);

    }
    else{
        int L1=cginfo->new_label();
        int L2=cginfo->new_label();
        expr->gen_code(table_stack);
    
        auto& os =*jout;
        print_tabs(os);
        os<<"ifeq L" <<L1<<endl;
        push_false();
        print_goto(L2);
        print_label(L1);
        push_true();
        print_label(L2);
    }
}

void BitNotExpr::check_semantic(const SymbolTableStack& table_stack)
{
    expr->check_semantic(table_stack);
    auto expr_info = expr->semantic_info;
    if (!(expr_info->compatible_with_int())) {
        ostringstream oss;
        oss << "No match for unary operation " << op->lexme << " " << expr_info->str();
        show_semantic_error(op, oss);
    }
    set_seman_info();
}

void BitNotExpr::gen_code(const SymbolTableStack& table_stack) const
{
    assert(false);
}

void PrefixIncExpr::check_semantic(const SymbolTableStack& table_stack)
{
    PreExpr::check_semantic(table_stack);
    this->semantic_info->is_const = false;

}

void print_const1(const Expr* expr) {
    print_const1(expr->semantic_info->type_name);
}

void gen_inc(const Expr* expr) {
    gen_inc(expr->semantic_info->type_name);
}
void gen_dec(const Expr* expr) {
    gen_dec(expr->semantic_info->type_name);
}

void PrefixIncExpr::gen_code(const SymbolTableStack& table_stack) const
{
    auto v = dynamic_cast<IdExpr*> (expr);
    if (v != nullptr) {
        auto decl =v->get_decl(table_stack);
        assert(decl!=nullptr);
        if (v->semantic_info->is_sub_type_of_int()&&
        (decl->scope==LOCAL_SCOPE
        ||decl->scope==PARA_SCOPE)
        ) 
        {
            ostringstream oss;
            assert(decl->symbol_index >= 0);
            oss <<"iinc "<<decl->symbol_index <<" 1";
            print_inst(oss.str());
            v->gen_code(table_stack);
        }
        else{
            v->gen_code(table_stack);
            gen_inc(expr);
            print_inst("dup");
            v->gen_store_inst(table_stack);
        }
        return;
    }

    auto a = dynamic_cast<ElementExpr*> (expr);
    if (a != nullptr) {
        a->addr->gen_code(table_stack);
        a->index->gen_code(table_stack);
        print_inst("dup2");
        a->gen_load_inst();
        gen_inc(a);
        print_inst("dup_x2");
        a->gen_store_inst();
        return;
    }
}

void PrefixDecExpr::check_semantic(const SymbolTableStack& table_stack)
{
    PreExpr::check_semantic(table_stack);
    this->semantic_info->is_const = false;
}

void PrefixDecExpr::gen_code(const SymbolTableStack& table_stack) const
{
    auto v = dynamic_cast<IdExpr*> (expr);
    if (v != nullptr) {
        v->gen_code(table_stack);
        gen_dec(expr);
        print_inst("dup");
        v->gen_store_inst(table_stack);
        return;
    }

    auto a = dynamic_cast<ElementExpr*> (expr);
    if (a != nullptr) {
        a->addr->gen_code(table_stack);
        a->index->gen_code(table_stack);
        print_inst("dup2");
        a->gen_load_inst();
        gen_dec(a);
        print_inst("dup_x2");
        a->gen_store_inst();
        return;
    }
    assert(false);
}

void PostExpr::check_semantic(const SymbolTableStack& table_stack)
{
    expr->check_semantic(table_stack);
    auto res_info = this->semantic_info;
    res_info->is_array = false;
    res_info->type = TYPE;

    auto expr_info = expr->semantic_info;
    if (!(expr_info->compatible_with_float())) {
        ostringstream oss;
        oss << "No match for " << expr_info->str() << " " << op->lexme;
        //oss << "operand for postfix " << op->lexme << " must be a number";
        show_semantic_error(op, oss);
    }

    if (expr_info->is_const) {
        ostringstream oss;
        auto p = dynamic_cast<IdExpr*>(expr);
        if (p != nullptr) {
            oss << "Cannot modify const variable '" << p->id->lexme << "'";
            show_semantic_error(op, oss);
        }

        auto p2 = dynamic_cast<ElementExpr*>(expr);
        if (p2 != nullptr) {
            auto p3 = dynamic_cast<IdExpr*>(p2->addr);
            if (p3 != nullptr) {
                oss << "Cannot modify const item " << p3->id->lexme << "[]";
                show_semantic_error(op, oss);
            }
        }

        oss << "can not modify const operand in postfix " << op->lexme;
        show_semantic_error(op, oss);
    }

    res_info->is_const = expr_info->is_const;
    res_info->type_name = expr_info->type_name;
}

void ArithExpr::check_semantic(const SymbolTableStack& table_stack)
{
    first->check_semantic(table_stack);
    auto info1 = first->semantic_info;
    second->check_semantic(table_stack);
    auto info2 = second->semantic_info;

    if (!(info1->compatible_with_float())
        || !(info2->compatible_with_float())
        ) {
        ostringstream oss;
        oss << "No match for binary operation " << info1->str() << " " << op->lexme << " " << info2->str();
        show_semantic_error(op, oss);
    }

    if (op->match(PERC)) {
        if (!(info1->compatible_with_int())
            || !(info2->compatible_with_int())
            ) {
            ostringstream oss;
            oss << "No match for binary operation " << info1->str() << " " << op->lexme << " " << info2->str();
            show_semantic_error(op, oss);
        }
    }

    auto res_info = this->semantic_info;
    res_info->is_array = false;
    res_info->type = TYPE;
    res_info->is_const = info1->is_const && info2->is_const;

    if (info1->type_name == "float" || info2->type_name == "float")
        res_info->type_name = "float";
    else if (info1->type_name == "int" || info2->type_name == "int")
        res_info->type_name = "int";
    else if (info1->type_name == "char" || info2->type_name == "char")
        res_info->type_name = "char";
}

void ArithExpr::gen_code(const SymbolTableStack& table_stack) const
{
    auto info1 = first->semantic_info;
    auto info2 = second->semantic_info;
    auto info = this->semantic_info;
    auto res_type = info->type_name;

    auto& os = *jout;
    if (debug_flag[5]) {
        print_tabs(os);
        os << ";; will gen first operand" << endl;
    }

    first->gen_code(table_stack);
    if (info1->type_name != res_type)
        gen_convert_inst(os, res_type, info1->type_name);
    if (debug_flag[5]) {
        print_tabs(os);
        os << ";; will gen second opreand" << endl;
    }
    second->gen_code(table_stack);
    if (info2->type_name != res_type)
        gen_convert_inst(os, res_type, info2->type_name);
    if (debug_flag[5]) {
        print_tabs(os);
        os << ";; after gen second opreand" << endl;
    }

    if (debug_flag[5]) {
        print_tabs(os);
        os << ";; will gen operator" << endl;
    }
    print_tabs(os);
    if (res_type == "float")
        os << "f";
    else
        os << "i";
    switch (op->type) {
    case PLUS:
        os << "add";
        break;
    case MINUS:
        os << "sub";
        break;
    case STAR:
        os << "mul";
        break;
    case DIV:
        os << "div";
        break;
    case PERC:
        os << "rem";
        break;
    default:
        assert(false);
        break;
    }
    os << endl;

    if (first->semantic_info->type_name == "char" && second->semantic_info->type_name == "char")
        print_inst("i2c");
}

void CompExpr::check_semantic(const SymbolTableStack& table_stack)
{
    first->check_semantic(table_stack);
    auto info1 = first->semantic_info;
    second->check_semantic(table_stack);
    auto info2 = second->semantic_info;

    if (!(info1->compatible_with_float())
        || !(info2->compatible_with_float())
        ) {
        ostringstream oss;
        oss << "No match for binary operation " << info1->str() << " " << op->lexme << " " << info2->str();
        show_semantic_error(op, oss);
    }

    auto res_info = this->semantic_info;
    res_info->is_array = false;
    res_info->type = TYPE;
    res_info->is_const = info1->is_const && info2->is_const;
    res_info->type_name = "char";
}

void CompExpr::gen_code(const SymbolTableStack& table_stack) const
{
    auto type1 = first->get_type();
    auto type2 = second->get_type();

    auto res_type =high_type(type1,type2);

    first->gen_code(table_stack);
    gen_convert_inst(*jout,res_type,type1);
    second->gen_code(table_stack);
    gen_convert_inst(*jout, res_type, type2);

    ostringstream oss;
    if (res_type=="float")
        oss<< "fcmpl";
    else
        oss <<"isub";
    print_inst(oss.str());
        
    auto& os = *jout;
    print_tabs(os);
    switch (op->type)
    {
    case EQ:
        os<< "ifeq";
        break;
    case NE:
        os << "ifne";
        break;
    case LT:
        os << "iflt";
        break;
    case GT:
        os << "ifgt";
        break;
    case GE:
        os << "ifge";
        break;
    case LE:
        os << "ifle";
        break;
    default:
        assert(false);
        break;
    }

    int L1 = cginfo->new_label();
    int L2 = cginfo->new_label();

    os << " L"<<L1<<endl;
    push_false();
    print_goto(L2);
    print_label(L1);
    push_true();
    print_label(L2);
}

void BitExpr::check_semantic(const SymbolTableStack& table_stack)
{
    first->check_semantic(table_stack);
    auto info1 = first->semantic_info;
    second->check_semantic(table_stack);
    auto info2 = second->semantic_info;

    if (!(info1->compatible_with_int())
        || !(info2->compatible_with_int())
        ) {
        ostringstream oss;
        oss << "No match for binary operation " << info1->str() << " " << op->lexme << " " << info2->str();
        show_semantic_error(op, oss);
    }

    auto res_info = this->semantic_info;
    res_info->is_array = false;
    res_info->type = TYPE;

    res_info->is_const = info1->is_const && info2->is_const;
    if (info1->type_name == "int" || info2->type_name == "int")
        res_info->type_name = "int";
    else if (info1->type_name == "char" || info2->type_name == "char")
        res_info->type_name = "char";
}

void AssignExpr::check_semantic(const SymbolTableStack& table_stack)
{
    first->check_semantic(table_stack);
    auto info1 = first->semantic_info;

    second->check_semantic(table_stack);
    auto info2 = second->semantic_info;

    if (info1->is_const) {
        ostringstream oss;
        auto p1 = dynamic_cast<IdExpr*>(first);
        if (p1 != nullptr) {
            oss << "Cannot assign to const variable '" << p1->id->lexme << "'";
            show_semantic_error(op, oss);
        }

        auto p2 = dynamic_cast<ElementExpr*>(first);
        if (p2 != nullptr) {
            auto p3 = dynamic_cast<IdExpr*>(p2->addr);
            if (p3 != nullptr) {
                oss << "Cannot assign to const item " << p3->token->lexme << "[]";
                //oss <<"Cannot modify const variable '"<<p1->token->lexme<<"'";
                show_semantic_error(op, oss);
            }
        }

        oss << "Assignment mismatch " << info1->str() << " " << op->lexme << " " << info2->str();
        show_semantic_error(op, oss);
    }

    if (info1->is_array || info2->is_array || info1->type != info2->type
        || (info1->type_name == "void")
        || (info1->type == STRUCT && info1->type_name != info2->type_name)) {
        ostringstream oss;
        oss << "Assignment mismatch " << info1->str() << " " << op->lexme << " " << info2->str();
        show_semantic_error(op, oss);
    }

    if (info1->type == TYPE) {
        if (!(info2->is_compatible_with_left(info1))) {
            ostringstream oss;
            oss << "Assignment mismatch " << info1->str() << " " << op->lexme << " " << info2->str();
            show_semantic_error(op, oss);
        }
    }

    this->semantic_info = info1;
}

void AssignExpr::gen_code(const SymbolTableStack& table_stack) const
{
    auto v = dynamic_cast<IdExpr*> (first);
    auto &os =*jout;
    if (v != nullptr) {
        if (op->type != ASSGIN) {
            //注意对 -= ,必须先把左边入栈,否则结果就会错误!
            first->gen_code(table_stack);
            second->gen_code(table_stack);
            gen_convert_inst(os, first->semantic_info->type_name, second->semantic_info->type_name);
            gen_op();
        }
        else {
            second->gen_code(table_stack);
            gen_convert_inst(os,first->semantic_info->type_name,second->semantic_info->type_name);
        }
        print_inst("dup");
        v->gen_store_inst(table_stack);
        return;
    }

    auto a = dynamic_cast<ElementExpr*>(first);
    if (a != nullptr) {
        if (op->type==ASSGIN){
            a->addr->gen_code(table_stack);
            a->index->gen_code(table_stack);
            second->gen_code(table_stack);
            gen_convert_inst(os, a->semantic_info->type_name, second->semantic_info->type_name);
        }
        else{
            a->addr->gen_code(table_stack);
            print_inst("dup");
            a->index->gen_code(table_stack);
            print_inst("dup_x1");
            a->gen_load_inst();
            second->gen_code(table_stack);
            gen_convert_inst(os, a->semantic_info->type_name, second->semantic_info->type_name);
            gen_op();
        }
        if (a->semantic_info->type_name == "char" &&op->type!=ASSGIN)
            print_inst("i2c");
        print_inst("dup_x2");
        a->gen_store_inst();
        return;
    }
    assert(false);
}

void AssignExpr::gen_op() const
{
    auto info1 = first->semantic_info;
    auto left_type = info1->type_name;

    auto& os = *jout;
    print_tabs(os);
    if (left_type == "float")
        os << "f";
    else
        os << "i";
    switch (op->type)
    {
    case PLUS_ASSIGN:
        os << "add";
        break;
    case MINUS_ASSIGN:
        os << "sub";
        break;
    case MUL_ASSIGN:
        os << "mul";
        break;
    case DIV_ASSIGN:
        os << "div";
        break;
    default:
        assert(false);
        break;
    }

    os << endl;
}

void CastExpr::check_semantic(const SymbolTableStack& table_stack)
{
    expr->check_semantic(table_stack);
    auto info1 = expr->semantic_info;

    if (!info1->compatible_with_float()) {
        ostringstream oss;
        oss << "Cannot cast expression of type " << info1->str() << " to " << type->lexme;
        show_semantic_error(type, oss);
    }

    auto res_info = this->semantic_info;
    res_info->is_array = false;
    res_info->type = TYPE;
    res_info->is_const = info1->is_const;
    res_info->type_name = type->lexme;
}

void CastExpr::gen_code(const SymbolTableStack& table_stack) const
{
    auto& os = *jout;
    expr->gen_code(table_stack);
    auto expr_info = expr->semantic_info;
    auto expr_type = expr_info->type_name;

    auto res_info = this->semantic_info;
    auto res_type = res_info->type_name;
    if (expr_type != res_type) {
        gen_convert_inst(os, res_type, expr_type);
    }
}

void ElementExpr::check_semantic(const SymbolTableStack& table_stack)
{
    addr->check_semantic(table_stack);
    auto info1 = addr->semantic_info;
    if (!info1->is_array) {
        ostringstream oss;
        auto p = dynamic_cast<TokenExpr*>(addr);
        if (p != nullptr) {
            auto var_name = p->token->lexme;
            oss << "Variable '" << var_name << "' is not an array";
        }
        else
            oss << "left side of " << lb->lexme << " must be an array";
        show_semantic_error(lb, oss);
    }

    index->check_semantic(table_stack);
    auto info2 = index->semantic_info;
    if (!info2->compatible_with_int()) {
        ostringstream oss;
        oss << "Array index should be integer, was " << info2->str();
        show_semantic_error(lb, oss);
    }

    auto res_info = this->semantic_info;
    res_info->is_array = false;
    res_info->type = info1->type;
    res_info->is_const = info1->is_const;
    res_info->type_name = info1->type_name;
}

void ElementExpr::gen_code(const SymbolTableStack& table_stack) const
{
    addr->gen_code(table_stack);
    index->gen_code(table_stack);

    gen_load_inst();
}

void ElementExpr::gen_load_inst() const
{
    auto type = this->semantic_info->type_name;
    auto& os = *jout;
    print_tabs(os);
    os << type[0] << "aload" << endl;
}

void ElementExpr::gen_store_inst() const
{
    auto type = this->semantic_info->type_name;
    auto& os = *jout;
    print_tabs(os);
    os << type[0];
    os << "astore" << endl;
}

void MemberExpr::check_semantic(const SymbolTableStack& table_stack)
{
    object->check_semantic(table_stack);
    auto info1 = object->semantic_info;

    if (info1->type != STRUCT || info1->is_array) {
        ostringstream oss;

        auto p1 = dynamic_cast<MemberExpr*>(object);
        if (p1 != nullptr)
        {
            auto father_name = p1->id;
            if (info1->is_array) {
                oss << "Member '" << father_name->lexme << "' is an array, cannot use member selector";
                show_semantic_error(op, oss);
            }
        }

        oss << "left side of " << op->lexme << " must be a struct symbol";
        show_semantic_error(op, oss);
    }

    auto p = table_stack.find_struct(info1->type_name);
    if (p == nullptr) {
        ostringstream oss;
        oss << "Unknown struct '" << info1->type_name << "'";
        //oss << "struct " << info1->type_name<<" does not exist" ;
        show_semantic_error(op, oss);
    }

    const auto name = this->id->lexme;
    if (debug_flag[4]) {
        cout << "member is" << name << endl;
    }

    //assert(info1->struct_decl!=nullptr);
    auto v = p->find_member(name);
    if (v == nullptr) {
        ostringstream oss;
        oss << "Unknown member '" << name << "' in struct " << info1->type_name;
        //oss << "member " << name << " doesnot exist in struct " <<info1->type_name;
        show_semantic_error(op, oss);
    }

    this->semantic_info = v->semantic_info;
}

void CallExpr::check_semantic(const SymbolTableStack& table_stack)
{
    int n2 = args->argNum();
    auto name = fun_name->lexme;
    auto f = table_stack.find_fun(name);
    if (f == nullptr) {
        if (mode >= 5) {
            if (name == "getchar" && n2 == 0) {
                this->semantic_info->is_array = false;
                this->semantic_info->is_const = false;
                this->semantic_info->type = TYPE;
                this->semantic_info->type_name = "int";
                return;
            }
            else if (name == "putchar") {
                if (n2 == 1) {
                    args->get(0)->check_semantic(table_stack);
                    auto arg_info = args->get(0)->semantic_info;
                    if (arg_info->compatible_with_int()) {
                        this->semantic_info->is_array = false;
                        this->semantic_info->is_const = false;
                        this->semantic_info->type = TYPE;
                        this->semantic_info->type_name = "int";
                        return;
                    }
                }
            }
        }

        ostringstream oss;
        oss << "Unknown function '" << name << "\'";
        show_semantic_error(fun_name, oss);
    }

    int n1 = f->paras->size();


    if (debug_flag[4]) {
        cout << "function " << name << " need " << n1 << " arguments" << endl;
        cout << "acutal given " << n2 << " args" << endl;
    }

    if (n1 != n2) {
        ostringstream oss;
        oss << "Parameter mismatch in call to " << f->proto_str() << "\n";
        oss << "\t" << "Expected " << n1 << ", received " << n2 << " parameters";
        show_semantic_error(fun_name, oss);
    }

    for (int i = 0;i < n1;++i) {
        auto para_info = (*(f->paras))[i]->semantic_info;
        args->get(i)->check_semantic(table_stack);
        auto arg_info = args->get(i)->semantic_info;
        if (!arg_info->is_compatible_with_para(para_info)) {
            ostringstream oss;
            oss << "Parameter mismatch in call to " << f->proto_str() << "\n";
            oss << "\t" << "Passed parameter #" << (i + 1) << " has type " << arg_info->str();
            show_semantic_error(fun_name, oss);
        }
    }

    this->semantic_info = f->semantic_info;
}

void CallExpr::gen_code(const SymbolTableStack& table_stack) const
{
    int n = args->argNum();

    const string& name = this->fun_name->lexme;

    auto& os = *jout;
    if (name == "getchar") {
        assert(n == 0);
        print_tabs(os);
        os << "invokestatic Method libc getchar ()I" << endl;
    }
    else if (name == "putchar") {
        args->get(0)->gen_code(table_stack);
        assert(n == 1);
        print_tabs(os);
        os << "invokestatic Method libc putchar (I)I" << endl;
    }
    else {
        assert(table_stack.size()==2);
        //cout << "will find fun" <<name<<endl;
        /*
        备注:测试用例中也可能调用只有声明没有定义的函数!

        */
        auto f = table_stack.find_fun(name);
        assert(f != nullptr);

        for (int i = 0;i < n;++i) {
            auto e = args->get(i);
            e->gen_code(table_stack);
            auto arg_info = e->semantic_info;
            if (!arg_info->is_array) {
                auto arg_type = arg_info->type_name;
                auto p = (*f->paras)[i];
                auto para_info = p->semantic_info;
                auto para_type = para_info->type_name;
                if (arg_type != para_type) {
                    gen_convert_inst(os, para_type, arg_type);
                }
            }
        }
        print_tabs(os);
        os << "invokestatic Method " << casename << " " << name << " " << f->utf_name << endl;
    }
}

void CondExpr::check_semantic(const SymbolTableStack& table_stack)
{
    cond->check_semantic(table_stack);
    true_branch->check_semantic(table_stack);
    false_branch->check_semantic(table_stack);

    auto info1 = cond->semantic_info;
    auto infoA = true_branch->semantic_info;
    auto infoB = false_branch->semantic_info;

    if (!info1->compatible_with_float()
        || infoA->type != infoB->type || infoA->is_array != infoB->is_array
        || (infoA->is_array && infoA->type_name != infoB->type_name)
        ) {
        ostringstream oss;

        oss << "No match for ternary operation " << info1->str() << " ? "
            << infoA->str() << " : " << infoB->str();
        //oss << "condition  of before ? must be a numer";
        show_semantic_error(qmark, oss);
    }

    auto res_info = this->semantic_info;

    res_info->is_array = infoA->is_array;

    if (res_info->is_array) {
        res_info->is_const = infoA->is_const || infoB->is_const;
    }
    else {
        res_info->is_const = infoA->is_const && infoB->is_const && info1->is_const;
    }

    if (infoA->type == STRUCT) {
        if (infoA->type_name != infoB->type_name)
        {
            ostringstream oss;
            oss << "two operands has different type";
            show_semantic_error(qmark, oss);
        }
        res_info->type = STRUCT;
        res_info->type_name = infoA->type_name;
    }
    else {
        res_info->type = TYPE;
        if (infoA->type_name == infoB->type_name)
        {
            res_info->type_name = infoA->type_name;
        }
        else {
            if (infoA->type_name == "void" || infoB->type_name == "void") {
                ostringstream oss;
                oss << "two operands has incompatible type";
                show_semantic_error(qmark, oss);
            }
            else if (infoA->type_name == "float" || infoB->type_name == "float")
                res_info->type_name = "float";
            else if (infoA->type_name == "int" || infoB->type_name == "int")
                res_info->type_name = "int";
            else if (infoA->type_name == "char" || infoB->type_name == "char")
                res_info->type_name = "char";
        }
    }
}

void CondExpr::gen_code(const SymbolTableStack& table_stack) const
{
    int L1=cginfo->new_label();
    int L2 = cginfo->new_label();

    auto& res_type= semantic_info->type_name;
    cond->gen_code(table_stack);
    auto &os =*jout;
    print_tabs(os);
    os << "ifeq L"<<L1<<endl;
    true_branch->gen_code(table_stack);
    gen_convert_inst(os,res_type,true_branch->semantic_info->type_name);
    print_goto(L2);
    print_label(L1);
    false_branch->gen_code(table_stack);
    gen_convert_inst(os, res_type, false_branch->semantic_info->type_name);
    print_label(L2);
}

void NegExpr::gen_code(const SymbolTableStack& table_stack) const
{
    expr->gen_code(table_stack);
    if (expr->semantic_info->type_name == "float")
        print_inst("fneg");
    else 
        print_inst("ineg");
    
    if (expr->semantic_info->type_name == "char")
        print_inst("i2c");
}

void PostIncExpr::gen_code(const SymbolTableStack& table_stack) const {
    auto v = dynamic_cast<IdExpr*> (expr);
    if (v != nullptr) {
        v->gen_code(table_stack);
        print_inst("dup");
        gen_inc(expr);
        v->gen_store_inst(table_stack);
        return;
    }

    auto a = dynamic_cast<ElementExpr*> (expr);
    if (a != nullptr) {
        a->addr->gen_code(table_stack);
        a->index->gen_code(table_stack);
        print_inst("dup2");
        a->gen_load_inst();
        print_inst("dup_x2");
        gen_inc(a);
        a->gen_store_inst();
        return;
    }
    assert(false);
}

void PostDecExpr::gen_code(const SymbolTableStack& table_stack) const {
    auto v = dynamic_cast<IdExpr*> (expr);
    if (v != nullptr) {
        v->gen_code(table_stack);
        print_inst("dup");
        gen_dec(expr);
        v->gen_store_inst(table_stack);
        return;
    }

    auto a = dynamic_cast<ElementExpr*> (expr);
    if (a != nullptr) {
        a->addr->gen_code(table_stack);
        a->index->gen_code(table_stack);
        print_inst("dup2");
        a->gen_load_inst();
        print_inst("dup_x2");
        gen_dec(a);
        a->gen_store_inst();
        return;
    }
    assert(false);
}

void BitAndExpr::gen_code(const SymbolTableStack& table_stack) const
{
    assert(false);
}

void BitOrExpr::gen_code(const SymbolTableStack& table_stack) const
{
    assert(false);
}

void LogicAndExpr::gen_code(const SymbolTableStack& table_stack) const
{
    int L1= cginfo->new_label();
    int L2 = cginfo->new_label();

    first->gen_code(table_stack);

    auto& os = *jout;
    print_tabs(os);
    os << "ifeq L" <<L1<<endl;
    second->gen_code(table_stack);
    os << "ifeq L" << L1 << endl;
    push_true();
    print_goto(L2);
    print_label(L1);
    push_false();
    print_label(L2);
}

void LogicOrExpr::gen_code(const SymbolTableStack& table_stack) const
{
    int L1 = cginfo->new_label();
    int L2 = cginfo->new_label();

    first->gen_code(table_stack);

    auto& os = *jout;
    print_tabs(os);
    os << "ifne L" << L1 << endl;
    second->gen_code(table_stack);
    os << "ifne L" << L1 << endl;
    push_false();
    print_goto(L2);
    print_label(L1);
    push_true();
    print_label(L2);
}
