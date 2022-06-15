#include "decl.h"
#include "table_stack.h"
#include "semantic_error.h"

void show_dup_decl(const SymbolDecl* v, const SymbolDecl* v2, Scope scope) {
    ostringstream oss;

    if (scope == GLOBAL_SCOPE) {
        oss << "Global variable '" << v->name->lexme <<
            "' duplicates global variable declared in" << endl;
    }
    else if (scope == LOCAL_SCOPE) {
        oss << "Local variable '" << v->name->lexme <<
            "' duplicates local variable declared in" << endl;
    }
    else if (scope == PARA_SCOPE) {
        oss << "Parameter '" << v->name->lexme << "' duplicates parameter declared in" << endl;
    }
    oss << "\t\t" << "file "
        << v2->name->pos.stream->filename
        << " line "
        << v2->name->pos.line_no
        ;
    show_semantic_error(v->name, oss);
}


void TypedSymbolDecl::set_seman_info(SymbolTableStack& table_stack)
{
    auto info = this->semantic_info;
    auto grammar_type = this->type_tokens;

    auto name = grammar_type->type->lexme;
    if (grammar_type->struct_token != nullptr) {
        //检查结构体的类型名是否存在
        assert(grammar_type->struct_token->type == STRUCT);
        auto p = table_stack.find_struct(name);
        if (p == nullptr) {
            ostringstream oss;
            oss << "Unknown struct '" << name << "'";
            //oss << "struct " << info->type_name << " doest not exist";
            show_semantic_error(grammar_type->type, oss);
        }

        if (debug_flag[4])
            cout << "struct symbol name is " << name << endl;

        info->type = STRUCT;
        //info->struct_decl = p;
    }
    else {
        info->type = TYPE;
    }

    info->is_const = (grammar_type->const_token != nullptr);
    info->type_name = name;
}


void VarDecl::pre_gen_code()
{
    assert(this->scope == GLOBAL_SCOPE);
    //auto info = this->semantic_info;

    #if 0
    auto name = this->semantic_info->type_name;
    if (name == "char")
        name = "int";
    char c = toupper(name[0]);
    ostringstream oss;
    oss <<c;
    #endif

    this->utf_name = this->semantic_info->get_utf();

    if (debug_flag[5]) {
        cout << "global var info:" << this->name->lexme << ", utf:" << utf_name << endl;
    }
}

void VarDecl::gen_init_code(const SymbolTableStack& table_stack) const
{
    auto& os = *jout;
    init_expr->gen_code(table_stack);
    auto info = this->semantic_info;
    auto expr_info = init_expr->semantic_info;
    auto expr_type = expr_info->type_name;
    auto res_type = info->type_name;
    if (res_type != expr_type)
        gen_convert_inst(os, res_type, expr_type);
    print_tabs(os);
    if (scope == LOCAL_SCOPE) {
        if (res_type == "float")
            os << "f";
        else
           os << "i";
        os << "store" ;
        assert(symbol_index>=0);
        if (symbol_index<4)
            os<<"_";
        else
            os <<" ";
        os << symbol_index << endl;
    }
    else if (scope == GLOBAL_SCOPE) {
        os << "putstatic Field " << casename << " " << name->lexme << " " << utf_name << endl;
    }
}

void VarDecl::check_semantic(SymbolTableStack& table_stack)
{
    //先检查本作用域是否有同名符号存在
    auto v = table_stack.find_dup_var(name->lexme);
    if (v != nullptr) {
        ostringstream oss;
        show_dup_decl(this, v, scope);
    }

    set_seman_info(table_stack);

    auto info = this->semantic_info;

    // void 不能作为变量类型
    if (info->type_name == "void") {
        ostringstream oss;

        oss << "Variable '" << name->lexme << "' has type void";
        show_semantic_error(name, oss);
    }

    info->is_array = false;

    // 检查表达式的类型是否符合预期
    if (init_expr != nullptr) {
        check_init(table_stack);
    }

    table_stack.add_symbol(this);
}

void VarDecl::check_init(const SymbolTableStack& table_stack)
{
    auto info = this->semantic_info;
    if (info->type == STRUCT) {
        ostringstream oss;
        oss << "cannot initialize the struct var ";
        show_semantic_error(name, oss);
    }

    init_expr->check_semantic(table_stack);
    auto expr_info = init_expr->semantic_info;
    assert(expr_info != nullptr);

    if (!expr_info->is_compatible_init(info)) {
        ostringstream oss;
        oss << "Initialization for \'" << this->name->lexme << "\' has wrong type: "
            << expr_info->str();
        show_semantic_error(name, oss);
    }
}

void VarDecl::print_semantic_info(int level, ostream& os) const
{
    ASTNode::print_semantic_info(level, os);

    switch (this->scope)
    {
    case GLOBAL_SCOPE:
        os << "global";
        break;
    case LOCAL_SCOPE:
        os << "local";
        break;
    case STRUCT_SCOPE:
        os << "member";
        break;
    case PARA_SCOPE:
        os << "parameter";
        break;
    default:
        break;
    }
    os << " ";

    if (this->type_tokens->const_token != nullptr) {
        os << "const ";
    }

    if (this->type_tokens->struct_token != nullptr)
        os << "struct ";
    os << this->type_tokens->type->lexme;
    os << " ";
    os << name->lexme;
    os << endl;
}

void ArrayDecl::pre_gen_code()
{   
    assert(this->scope==GLOBAL_SCOPE);
    
    //auto info = this->semantic_info;
    #if 0
    ostringstream oss;
    oss << "[";
    auto name = this->semantic_info->type_name;
    char c = toupper(name[0]);
    oss << c;
    #endif

    this->utf_name = this->semantic_info->get_utf();

    if (debug_flag[5]) {
        cout << "array cg info:" << this->name->lexme << ", utf:" << utf_name << endl;
    }
}

void ArrayDecl::gen_init_code() const
{
    if (size != nullptr) {
        auto res_info = this->semantic_info;
        auto& os =*jout;
        int x = my_atoi(size->lexme);
        print_tabs(os);
        os << "ldc " << x << endl;
        print_tabs(os);
        auto type = res_info->type_name;
        os << "newarray " << type << endl;
        print_tabs(os);
        if (scope==GLOBAL_SCOPE){
            os << "putstatic Field " << casename << " " << name->lexme << " " << utf_name << endl;
        }
        else if (scope == LOCAL_SCOPE) {
            
            os << "astore" ;
            if (this->symbol_index<4)
                os<<"_";
            else
                os << " ";
            os<< this->symbol_index << endl;
        }
    }
}

void ArrayDecl::check_semantic(SymbolTableStack& table_stack)
{
    //先检查本作用域是否有同名符号存在
    auto v = table_stack.find_dup_var(name->lexme);
    if (v != nullptr) {
        ostringstream oss;
        show_dup_decl(this, v, scope);
    }

    set_seman_info(table_stack);

    auto info = this->semantic_info;

    // void 不能作为变量类型
    if (info->type_name == "void") {
        ostringstream oss;

        oss << "Variable '" << name->lexme << "' has type void";
        show_semantic_error(name, oss);
    }

    info->is_array = true;
    table_stack.add_symbol(this);
}

void ArrayDecl::print_semantic_info(int level, ostream& os) const
{
    ASTNode::print_semantic_info(level, os);

    switch (this->scope)
    {
    case GLOBAL_SCOPE:
        os << "global";
        break;
    case LOCAL_SCOPE:
        os << "local";
        break;
    case STRUCT_SCOPE:
        os << "member";
        break;
    case PARA_SCOPE:
        os << "parameter";
        break;
    default:
        break;
    }
    os << " ";

    if (this->type_tokens->const_token != nullptr) {
        os << "const ";
    }

    if (this->type_tokens->struct_token != nullptr)
        os << "struct ";
    os << this->type_tokens->type->lexme;
    os << " ";
    os << name->lexme;
    os << "[";
    if (size != nullptr)
        os << (size->lexme);
    os << "]";
    os << endl;
}

void StructDecl::check_semantic(SymbolTableStack& table_stack)
{
    auto p = table_stack.find_dup_struct(name->lexme);
    if (p != nullptr) {
        ostringstream oss;
        oss << "Struct '" << name->lexme << "' already declared in file ";
        oss << p->name->pos.stream->filename;
        oss << " line " << p->name->pos.line_no;
        show_semantic_error(name, oss);
    }

    table_stack.push_back(&member_table);
    for (auto e : *members)
        e->check_semantic(table_stack);
    table_stack.pop_back();

    table_stack.add_symbol(this);
}

void StructDecl::print_semantic_info(int level, ostream& os) const
{
    ASTNode::print_semantic_info(level, os);
    switch (this->scope)
    {
    case GLOBAL_SCOPE:
        os << "global";
        break;
    default:
        os << "local";
        break;
    }
    os << " ";
    os << "struct " << (name->lexme) << endl;
    for (auto e : *members) {
        e->print_semantic_info(level + 1, os);
    }
}

const TypedSymbolDecl* StructDecl::find_member(const string& name) const
{
    const TypedSymbolDecl* p = members->find_var(name);
    if (p == nullptr)
        p = members->find_array(name);
    return p;
}

void FunDecl::pre_gen_code()
{
    // 确定所有参数和局部变量的编号
    int n = paras->size();
    var_num = 0;
    for (int i = 0;i < n;++i) {
        auto  p = (*paras)[i];
        p->symbol_index = var_num++;
        all_vars.push_back(p);
    }
    
    if (body!=nullptr){
        for (auto e : *(body->decls)) {
                auto v =dynamic_cast<TypedSymbolDecl*>(e);
                assert(v!=nullptr);
                all_vars.push_back(e);
                v->symbol_index = var_num++;
        }
    }

    ostringstream oss;
    oss << "(";
    for (auto e : *paras) {
        oss <<e->semantic_info->get_utf();
    }
    oss << ")";
    
    auto fun_info = this->semantic_info;
    oss<<fun_info->get_utf();
    this->utf_name = oss.str();

    if (debug_flag[5]) {
        cout << "fun cg info:" << this->name->lexme << ", utf:" << utf_name << endl;
        cout << " local : " << var_num << endl;
        for (auto e : *paras) {
            cout << "para " << e->name->lexme << " at offset " << e->symbol_index << endl;
        }
    }

}

void FunDecl::gen_code(const SymbolTableStack& table_stack) const
{
    auto& os=*jout;
    os << endl;
    os << ".method public static " << name->lexme << " : " << utf_name << endl;
    os << "\t.code stack 1024 locals " << var_num << endl;

    gen_comment_for_vars();
    auto return_flag = body->gen_code(table_stack);
    check_return(return_flag);

    os << "\t.end code" << endl;
    os << ".end method" << endl;
    
}

void FunDecl::check_return(bool return_flag) const
{
    if (!return_flag) {
        auto fun_info = this->semantic_info;
        if (fun_info->type_name == "void") {
            print_tabs(*jout);
            (*jout) << "return ; automatically added" << endl;
        }
        else {
            cerr << "Code generation error in file "
                << body->rbrace->get_filename()
                << " line "
                << body->rbrace->get_line_no()
                << endl;
            cerr << "\tMissing return in non-void function " <<name->lexme<< endl;
            exit(-1);
        }
    }
}

void FunDecl::gen_comment_for_vars() const
{
    auto& os = *jout;
    for (auto x : all_vars) {
        auto e =dynamic_cast<TypedSymbolDecl*>(x);
        assert(e!=nullptr);
        print_tabs(os);
        assert(e->symbol_index >= 0);
        os << ";; local " << e->symbol_index << " " << e->name->lexme
            << " " << e->name->get_filename()
            << " " << e->name->get_line_no()
            << endl;
    }
}

void FunDecl::check_semantic(SymbolTableStack& table_stack)
{
    const string& fun_name = this->name->lexme;

    if (debug_flag[4]) {
        cout << "check_semantic for head of function : " << fun_name << endl;
    }

    auto p = table_stack.find_fun(fun_name);
    if (p != nullptr) {
        //检查函数是否被重复定义
        if (p->body != nullptr && this->body != nullptr)
        {
            ostringstream oss;
            oss << "Function '" << fun_name << "' already defined in file ";
            oss << p->name->pos.stream->filename;
            oss << " line " << p->name->pos.line_no;
            show_semantic_error(name, oss);
        }
    }

    set_seman_info(table_stack);


    table_stack.current_fun = this;

    table_stack.push_back(&table);
    for (auto e : *paras)
        e->check_semantic(table_stack);
    table_stack.pop_back();

    table_stack.add_symbol(this);

    //检查类型一致的工作必须要等函数头分析完才能进行!
    if (p != nullptr) {
        if (!check_same_proto(p)) {
            ostringstream oss;
            /*
            Function int other_function(float)
            conflicts with earlier prototype
            float other_function(float)
            defined in file proto1.c line 2
            */
            if (body != nullptr)
                oss << "Function ";
            else
                oss << "Prototype ";
            oss << proto_str() << endl;
            oss << "\t" << "conflicts with earlier prototype" << endl;
            oss << "\t" << p->proto_str() << endl;
            oss << "\t" << "defined in file ";
            oss << p->name->pos.stream->filename;
            oss << " line " << p->name->pos.line_no << endl;
            show_semantic_error(name, oss);
        }
    }

    if (body != nullptr) {
        if (debug_flag[4]) {
            cout << "check_semantic for body of function : " << name->lexme<< endl;
        } 
        table_stack.push_back(&table);
        body->check_semantic(table_stack);
        table_stack.pop_back();
    }
    table_stack.current_fun = nullptr;
}

void FunDecl::print_semantic_info(int level, ostream& os) const
{
    if (body != nullptr) {
        ASTNode::print_semantic_info(level, os);
        os << "function ";
        auto info = this->semantic_info;
        if (info->type == STRUCT)
            os << "struct ";
        os << info->type_name << " ";
        os << name->lexme;
        os << endl;

        for (const auto e : *paras) {
            e->print_semantic_info(level + 1, os);
        }

        body->print_semantic_info(level + 1, os);
    }
}

string FunDecl::proto_str() const
{
    ostringstream oss;
    oss << semantic_info->str() << " " << name->lexme << "(";
    int n = paras->size();
    if (n > 0) {
        oss << (*paras)[0]->semantic_info->str();
        for (int i = 1;i < n;++i)
            oss << ", " << (*paras)[i]->semantic_info->str();
    }
    oss << ")";
    return oss.str();
}

bool FunDecl::check_same_proto(const FunDecl* other) const
{
    return this->semantic_info->is_same_return_type(other->semantic_info)
        && check_same_paras(other);
}

bool FunDecl::check_same_paras(const FunDecl* other) const
{
    auto parasA = this->paras;
    auto parasB = other->paras;

    if (parasA->size() != parasB->size())
    {
        return false;
    }

    int n = parasA->size();
    for (int i = 0;i < n;++i) {
        auto p1 = (*parasA)[i]->semantic_info;
        auto p2 = (*parasB)[i]->semantic_info;

        if (!(p1->is_same_para_type(p2))) {
            return false;
        }
    }

    return true;
}

bool FuncBody::gen_code(const SymbolTableStack& table_stack) const
{
    bool has_return = false;
    for (auto e : *objs) {
        auto v =dynamic_cast<VarDecl*> (e);
        if (v != nullptr && v->init_expr!=nullptr) {
            v->gen_init_code(table_stack);
            continue;
        }

        auto a = dynamic_cast<ArrayDecl*> (e);
        if (a!=nullptr){
            a->gen_init_code();
            continue;
        }
        auto s = dynamic_cast<Statement*> (e);
        if (s != nullptr) {
            s->gen_code(table_stack);
            auto r = dynamic_cast<const ReturnStatement*>(e);
            if (r != nullptr) {
                has_return = true;
                break;
            }
        }
    }
    return has_return;
    
}

void FuncBody::check_semantic(SymbolTableStack& table_stack)
{
    
    for (auto e : *objs) {
        auto d = dynamic_cast<SymbolDecl*> (e);
        if (d != nullptr)
            d->check_semantic(table_stack);
        else {
            auto s = dynamic_cast<Statement*> (e);
            if (s != nullptr)
                s->check_semantic(table_stack);
        }
    }
}

void FuncBody::print_semantic_info(int level, ostream& os) const
{
    for (const auto e : *objs)
        e->print_semantic_info(1, os);
}
