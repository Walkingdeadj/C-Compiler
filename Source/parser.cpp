#include "lexer.h"
#include "cginfo.h"
#include "table_stack.h"
#include "parser.h"

void show_parser_error(const Token* t, const string& des) {

    cerr << "Parser error in file "
        << (t->pos.stream->filename)
        << " line "
        << t->pos.line_no
        << " near text "
        << t->lexme
        ;

    cerr << "\n";

    cerr << '\t' << des << "\n";

    success = false;
    throw success;
}

void show_parser_error(const Token* t, const ostringstream& oss) {
    show_parser_error(t, oss.str());
}

void show_unmatch(const Token* t) {
    assert(t->match(LP) || t->match(LBRACKET) || t->match(LBRACE));
    ostringstream oss;
    oss << "Unmatched " << '\'' << t->lexme << '\'';
    show_parser_error(t, oss);
}


//如果下一个词法元素的类型不符合预期,则回退之
bool Parser::next_match(TokenType type) {
    if (!has_next())
        return false;
    if (next_token()->match(type))
        return true;
    else {
        unget(last_token);
        return false;
    }
}

bool Parser::next_match(const set<TokenType>& types) {
    if (!input.has_next())
        return false;
    if (next_token()->match(types))
        return true;
    else {
        unget(last_token);
        return false;
    }
}

const Token* Parser::expect_semi()
{
    return expect(SEMICOLON, ';');
}

const Token* Parser::expect_id()
{
    return expect(ID, "identifier");
}

const Token* Parser::expect_lp()
{
    return expect(LP, '(');
}

const Token* Parser::expect_rp(const Token* t) {
    return expect(RP, ')');
}

const Token* Parser::expect_rb(const Token* t) {
    return expect(RBRACKET, ']');
}

const Token* Parser::expect_rc(const Token* t) {
    if (!next_match(RBRACE)) {
        cerr << "Parser error in file "
            << (t->pos.stream->filename)
            << " line "
            << t->pos.line_no
            ;

        cerr << "\n";

        cerr << '\t' << "Unmatched '{'" << "\n";

        success = false;
        throw success;
    }

    return last_token;
}

const Token* Parser::expect_expr_start()
{
    if (!next_match(expr_start)) {
        ostringstream oss;
        oss << "term expected";
        show_parser_error(last_token, oss);
    }
    return last_token;
}

const Token* Parser::expect(TokenType type, char c)
{
    if (!next_match(type)) {
        ostringstream oss;
        oss << "Expected \'" << c << '\'';
        show_parser_error(last_token, oss);
    }
    return last_token;
}

const Token* Parser::expect(TokenType type, const string& s)
{
    if (!next_match(type)) {
        ostringstream oss;
        oss << "Expected " << s;
        show_parser_error(last_token, oss);
    }
    return last_token;
}

const Token* Parser::next_token() {
    if (input.has_next()) {
        last_token = input.move_next();
        return last_token;
    }
    else {
        return nullptr;
    }
}

void Parser::unget(const Token* t) {
    input.unget(t);
}

Expr* Parser::parse_expr(const Token* t) {
    unget(t);
    return parse_expr();
}

Expr* Parser::parse_expr() {
    Expr* res = nullptr;
#if 1
    expect_expr_start();
    auto t = last_token;
    if (debug_flag[3]) {
        cout << "term expected" << (t->lexme) << endl;
    }
    unget(t);
    res = parse_assign_expr();
#endif
    return res;
}

/*
primary-expression:
      identifier
    | constant
    | string-literal
    |'('  expression   ')'

*/
Expr* Parser::parse_term() {
    if (!has_next()) {
        ostringstream oss;
        oss << "expect term";
        show_parser_error(last_token, oss);
    }

    const Token* t = next_token();
    if (debug_flag[3]) {
        cout << "start token of term:" << (t->lexme) << endl;
    }
    Expr* e = nullptr;

    switch (t->type)
    {
    case ID:
        e = new IdExpr(t);
        break;
    case INT_LIT:
        e = new IntExpr(t);
        break;
    case CHAR_LIT:
        e = new CharExpr(t);
        break;
    case STRING_LIT:
        e = new StringExpr(t);
        break;
    case REAL_LIT: {
        e = new FloatExpr(t);
        break;
    }
    case LP:
    {
        auto body = parse_expr();
        expect_rp(t);
        e = body;
        break;
    }
    default: {
        ostringstream oss;
        oss << "term expected";
        show_parser_error(t, oss.str());
        break;
    }
    }
    return e;
}

/*
postfix-expression:
primary-expression ('[' expression ']' | (' (argument-expression-list)?  ')'  |  '.' identifier
    | '++' | '--') *
*/
Expr* Parser::parse_postfix_expr() {
    Expr* e = parse_term();
    bool stop_flag = false;
    while (has_next() && !stop_flag)
    {
        auto t = next_token();
        switch (t->type)
        {
        case LBRACKET:
        {
            auto index = parse_expr();
            auto rb = expect_rb(t);
            e = new ElementExpr(e, t, index, rb);
            break;
        }
        case LP: {
            auto args = parse_args();
            auto rp = expect_rp(t);
            IdExpr* id = dynamic_cast<IdExpr*>(e);
            if (id == nullptr) {
                throw ("invalid fun call expr");
            }
            e = new CallExpr(id->id, t, args, rp);
            break;
        }
        case DOT: {
            auto id = expect_id();
            e = new MemberExpr(e, t, id);
            break;
        }
        case INC:
        {
            if (e->is_lvalue()) {
                e = new PostIncExpr(e, t);
            }
            else {
                show_parser_error(t, "Expected ';'");
            }
            break;
        }
        case DEC:
        {
            if (e->is_lvalue()) {
                e = new PostDecExpr(e, t);
            }
            else {
                show_parser_error(t, "Expected ';'");
            }
            break;
        }
        default: {
            stop_flag = true;
            unget(t);
            break;
        }
        }
    }
    return e;
}

Arguments* Parser::parse_args()
{
    auto p = new Arguments();
    if (next_match(expr_start)) {
        unget(last_token);
        p->push(parse_assign_expr());
        while (has_next()) {
            if (next_match(COMMA)) {
                p->push(last_token);
                p->push(parse_assign_expr());
            }
            else if (next_match(RP)) {
                unget(last_token);
                break;
            }
            else {
                show_parser_error(last_token, "Expected ','");
            }
        }
    }
    else if (next_match(RP)) {
        unget(last_token);
    }
    else {
        show_parser_error(last_token, "term expected");
    }
    return p;
}

/*
assignment-expression:
    conditional-expression (assignment-operator assignment-expression)?
    assignment-operator: one of = *= /= %= += -= <<= >>= &= ^= |=
*/
Expr* Parser::parse_assign_expr() {
    Expr* e = nullptr;
#if 1
    e = parse_cond_expr();
    if (next_match(assign_ops))
    {
        if (!e->is_lvalue())
            show_parser_error(last_token, "Expected ';'");
        auto op = last_token;
        auto p = parse_assign_expr();
        e = new AssignExpr(e, op, p);
    }
#endif
    return e;
}

/*
unary-expression:
    postfix-expression
    |'++' unary-expression
    |'--' unary-expression
    |unary-operator cast-expression
    |'sizeof' unary-expression
    |'sizeof' '(’ type-name ')'
*/

Expr* Parser::parse_unary_expr() {
    expect_expr_start();
    Expr* e = nullptr;
    auto t = last_token;
    if (debug_flag[3]) {
        cout << "start token of unary expr:" << (t->lexme) << endl;
    }
    switch (t->type)
    {
    case INC: {
        auto p = parse_unary_expr();
        e = new PrefixIncExpr(t, p);
        break;
    }
    case DEC: {
        auto p = parse_unary_expr();
        e = new PrefixDecExpr(t, p);
        break;
    }
    case MINUS:
    {
        auto p = parse_cast_expr();
        e = new NegExpr(t, p);
        break;
    }
    case NOT: {
        auto p = parse_cast_expr();
        e = new NotExpr(t, p);
        break;
    }
    case BIT_NOT:
    {
        auto p = parse_cast_expr();
        e = new BitNotExpr(t, p);
        break;
    }
    default:
        unget(t);
        e = parse_postfix_expr();
        break;
    }
    return e;
}

/*
cast-expression:
    unary-expression
    | '(' type-name   ')' cast-expression
*/
Expr* Parser::parse_cast_expr() {
    auto t = expect_expr_start();
    Expr* e = nullptr;
    if (debug_flag[3]) {
        cout << "start token of cast expr:" << (t->lexme) << endl;
    }
    if (!t->match(LP)) {
        unget(t);
        e = parse_unary_expr();
    }
    else {
        if (!has_next()) {
            ostringstream oss;
            oss << "expect type";
            show_parser_error(t, oss);
        }
        if (next_match(TYPE)) {
            auto type = last_token;
            auto rp = expect_rp(t);
            auto expr = parse_cast_expr();
            e = new CastExpr(t, type, rp, expr);
        }
        else {
            unget(t);
            e = parse_unary_expr();
        }
    }
    return e;
}

/*
multiplicative-expression:
    cast-expression  ('*' | '/' | '%' cast-expression )*
*/
Expr* Parser::parse_mul_expr() {
    Expr* e = parse_cast_expr();
    while (next_match(mul_ops))
    {
        auto op = last_token;
        auto p = parse_cast_expr();
        e = new MulExpr(e, op, p);
    }
    return e;
}

/*
additive-expression:
    multiplicative-expression ( ('+'|'-')  multiplicative-expression)*
*/
Expr* Parser::parse_add_expr() {
    Expr* e = parse_mul_expr();
    while (next_match(add_ops))
    {
        auto op = last_token;
        auto p = parse_mul_expr();
        e = new AddExpr(e, op, p);
    }
    return e;
}

/*
shift-expression:
    additive-expression ( '<<' | '>>' additive-expression)*
*/
Expr* Parser::parse_shift_expr() {
    return parse_add_expr();
}

/*
relational-expression:
    shift-expression  ('<' | '>' | '<=' | '>='  shift-expression)*
*/
Expr* Parser::parse_rel_expr() {
    Expr* e = parse_shift_expr();
    while (next_match(relation_ops))
    {
        auto op = last_token;
        auto p = parse_shift_expr();
        e = new RelExpr(e, op, p);
    }
    return e;
}
/*
equality-expression:
    relational-expression （'=='| '!=' relational-expression)*
*/
Expr* Parser::parse_eq_expr() {
    Expr* e = parse_rel_expr();
    while (next_match(eq_ops))
    {
        auto op = last_token;
        auto p = parse_rel_expr();
        e = new EqExpr(e, op, p);
    }
    return e;
}

/*
AND-expression:
    equality-expression (& equality-expression)*
*/
Expr* Parser::parse_bit_and_expr() {
    Expr* e = parse_eq_expr();
    while (next_match(BIT_AND))
    {
        auto op = last_token;
        auto p = parse_eq_expr();
        e = new BitAndExpr(e, op, p);
    }
    return e;
}

/*
exclusive-OR-expression:
    AND-expression ( '^' AND-expression)*
*/

Expr* Parser::parse_bit_xor_expr() {
    return parse_bit_and_expr();
}

/*
inclusive - OR - expression:
    exclusive - OR - expression('|' exclusive - OR - expression) *
*/
Expr* Parser::parse_bit_or_expr() {
    Expr* e = parse_bit_xor_expr();
    while (next_match(BIT_OR))
    {
        auto op = last_token;
        auto p = parse_bit_xor_expr();
        e = new BitOrExpr(e, op, p);
    }
    return e;
}

/*
logical-AND-expression:
    inclusive-OR-expression ('&&' inclusive-OR-expression)*
*/

Expr* Parser::parse_logic_and_expr() {
    Expr* e = parse_bit_or_expr();
    while (next_match(AND))
    {
        auto op = last_token;
        auto p = parse_bit_or_expr();
        e = new LogicAndExpr(e, op, p);
    }
    return e;
}

/*
logical-OR-expression:
    logical-AND-expression ('||' logical-AND-expression)*
*/
Expr* Parser::parse_logic_or_expr() {
    Expr* e = parse_logic_and_expr();
    while (next_match(OR))
    {
        auto op = last_token;
        auto p = parse_logic_and_expr();
        e = new LogicOrExpr(e, op, p);
    }
    return e;
}


/*
* conditional-expression:
    logical-OR-expression   ( '?' expression ':' conditional-expression )?
*/
Expr* Parser::parse_cond_expr() {
    Expr* e = parse_logic_or_expr();
    if (next_match(QN_MARK)) {
        auto t1 = last_token;
        auto p1 = parse_expr();
        auto t2 = expect(COLON, ':');
        auto p2 = parse_cond_expr();
        e = new CondExpr(e, t1, p1, t2, p2);
    }
    return e;
}

SymbolTable* Parser::do_parse() {
    auto p = parse_decls(GLOBAL_SCOPE);
    if (has_next()) {
        auto t = next_token();
        ostringstream oss;
        oss << "unexpected token " << t->lexme;
        show_parser_error(t, oss);
    }
    program = p;
    return p;
}

SymbolTable* Parser::parse_decls(Scope scope) {
    auto decls = new SymbolTable();
    while (parse_decl(decls, scope))
    {
        ;
    }
    return decls;
}

bool Parser::parse_decl(vector<SymbolDecl*>* decls, Scope scope) {
    if (!has_next())
        return false;
    auto t = next_token();

    if (debug_flag[3]) {
        cout << "start token of decl :" << t->lexme << endl;
    }

    switch (t->type)
    {
    case TYPE:
        parse_decl_start_type(t, decls, scope);
        break;
    case CONST:
        parse_decl_start_const(t, decls, scope);
        break;
    case STRUCT:
        parse_decl_start_struct(t, decls, scope);
        break;
    default:
    {
        unget(t);
        if (scope == GLOBAL_SCOPE || (scope == STRUCT_SCOPE && !t->match(RBRACE)))
        {
            show_parser_error(t, "Expected type name");
        }
        return false;
        break;
    }
    }
    return true;
}

SymbolTypeInfo* Parser::parse_type_info_start_type(const Token* t) {
    SymbolTypeInfo* p = nullptr;
    if (next_match(CONST)) {
        p = new SymbolTypeInfo(last_token, nullptr, t);
    }
    else {
        p = new SymbolTypeInfo(nullptr, nullptr, t);
    }

    if (debug_flag[3]) {
        //p->print(cout);
    }

    return p;
}

SymbolTypeInfo* Parser::parse_type_info_start_const(const Token* t) {
    SymbolTypeInfo* p = nullptr;
    if (next_match(TYPE)) {
        p = new SymbolTypeInfo(t, nullptr, last_token);
    }
    else if (next_match(STRUCT)) {
        auto t1 = last_token;
        auto id = expect_id();
        p = new SymbolTypeInfo(t, t1, id);
    }
    return p;
}

void Parser::parse_decl_start_type(const Token* t, vector<SymbolDecl*>* decls, Scope scope) {
    auto type_info = parse_type_info_start_type(t);
    parse_decl_body(type_info, decls, scope);
}

void Parser::parse_decl_start_const(const Token* t, vector<SymbolDecl*>* decls, Scope scope) {
    auto type_info = parse_type_info_start_const(t);
    parse_decl_body(type_info, decls, scope);
}

void Parser::parse_decl_start_struct(const Token* struct_token
    , vector<SymbolDecl*>* decls
    , Scope scope
) {
    auto id = expect_id();
    auto t = next_token();
    switch (t->type)
    {
    case CONST:
    {
        auto type_tokens = new SymbolTypeInfo(t, struct_token, t);
        parse_decl_body(type_tokens, decls, scope);
        break;
    }
    case LBRACE:
    {
        auto p = parse_struct_members(struct_token, id, last_token);
        assert(p != nullptr);
        decls->push_back(p);
        break;
    }
    case ID: {
        auto type_tokens = new SymbolTypeInfo(nullptr, struct_token, id);
        unget(t);
        parse_decl_body(type_tokens, decls, scope);
        break;
    }
    default: {
        ostringstream oss;
        oss << "expect id";
        show_parser_error(last_token, oss);
        break;
    }
    }
}

void Parser::parse_decl_body(SymbolTypeInfo* type, vector<SymbolDecl*>* decls, Scope scope) {
    auto id = expect_id();
    if (next_match(LP)) {
        if (scope != GLOBAL_SCOPE) {
            ostringstream oss;
            oss << "Expected ';'";
            show_parser_error(last_token, oss);
        }
        auto fun = parse_fundecl(type, id, last_token);
        assert(fun != nullptr);
        decls->push_back(fun);
    }
    else {
        unget(id);
        parse_symbol_decls(type, decls);
    }
}

void Parser::parse_symbol_decls(SymbolTypeInfo* type, vector<SymbolDecl*>* decls) {
    do {
        auto id = expect_id();
        auto v = parse_symbol_decl(type, id);
        assert(v != nullptr);
        decls->push_back(v);
    } while (next_match(COMMA));
    expect_semi();
}

TypedSymbolDecl* Parser::parse_symbol_decl(SymbolTypeInfo* type, const Token* id) {
    auto t = next_token();
    TypedSymbolDecl* d = nullptr;
    switch (t->type) {
    case ASSGIN: {
        Expr* expr = parse_assign_expr();
        d = new VarDecl(type, id, t, expr);
        break;
    }
    case LP://函数声明
    {
        ostringstream oss;
        oss << "can define inner function";
        show_parser_error(last_token, oss);
        break;
    }
    case LBRACKET: {
        const Token* size = nullptr;
        if (next_match(INT_LIT)) {
            size = last_token;
        }
        auto rb = expect_rb(t);
        d = new ArrayDecl(type, id, t, size, rb);
        break;
    }
    default:
        unget(t);
        d = new VarDecl(type, id);
        break;
    }

    return d;
}

StructDecl* Parser::parse_struct_members(const Token* t, const Token* id, const Token* lc) {
    auto decls = parse_decls(STRUCT_SCOPE);
    auto rc = expect_rc(lc);
    auto semi = expect_semi();
    return new StructDecl(t, id, lc, decls, rc, semi);
}

FunDecl* Parser::parse_fundecl(SymbolTypeInfo* type, const Token* id, const Token* lp) {
    FunDecl* fun = nullptr;
    auto paras = parse_paras();
    auto rp = expect_rp(lp);

    if (next_match(SEMICOLON)) {
        fun = new FunDecl(type, id, lp, paras, rp, last_token);
    }
    else if (next_match(LBRACE)) {
        auto body = parse_funbody(last_token);
        fun = new FunDecl(type, id, lp, paras, rp, body);
    }
    else {
        show_parser_error(last_token, "expect ';'");
    }
    assert(fun != nullptr);
    return fun;
}

vector<TypedSymbolDecl*>* Parser::parse_paras() {
    auto decls = new vector<TypedSymbolDecl*>();
    if (next_match(type_start)) {
        auto v = parse_para(last_token);
        assert(v != nullptr);
        decls->push_back(v);
        while (next_match(COMMA)) {
            if (!next_match(type_start)) {
                show_parser_error(last_token, "Expected type name");
            }
            auto p = parse_para(last_token);
            assert(p != nullptr);
            decls->push_back(p);
        }
    }
    else if (next_match(RP)) {
        unget(last_token);
    }
    else {
        show_parser_error(last_token, "Expected type name");
    }

    return decls;
}

TypedSymbolDecl* Parser::parse_para(const Token* start) {
    auto type = parse_para_type_info(start);
    auto id = expect_id();
    TypedSymbolDecl* v = nullptr;
    if (next_match(LBRACKET)) {
        auto lb = last_token;
        auto rb = expect_rb(lb);

        //cout <<"find an array para" <<endl;

        v = new ArrayDecl(type, id, lb, nullptr, rb);
    }
    else if (next_match(RP) || next_match(COMMA)) {
        unget(last_token);
        v = new VarDecl(type, id);
    }
    else {
        show_parser_error(last_token, "Expected ','");
    }
    return v;
}

SymbolTypeInfo* Parser::parse_para_type_info(const Token* start) {
    const Token* struct_token = nullptr;
    const Token* id = nullptr;
    const Token* const_token = nullptr;
    switch (start->type)
    {
    case TYPE: {
        id = start;
        if (next_match(CONST)) {
            const_token = last_token;
        }
        break;
    }
    case STRUCT: {
        struct_token = start;
        if (!next_match(ID)) {
            ostringstream oss;
            oss << "expect id";
            show_parser_error(last_token, oss);
        }
        id = last_token;
        if (next_match(CONST)) {
            const_token = last_token;
        }
        break;
    }
    case CONST: {
        const_token = start;
        if (next_match(TYPE)) {
            id = last_token;
        }
        else if (next_match(STRUCT)) {
            struct_token = last_token;
            id = expect_id();
        }
        else {
            show_parser_error(last_token, "Expected type name");
        }
        break;
    }
    default: {
        break;
    }
    }

    auto type_tokens = new SymbolTypeInfo(const_token, struct_token, id);
    return type_tokens;
}

FuncBody* Parser::parse_funbody(const Token* lc) {
    auto decls = new vector<SymbolDecl*>();
    auto stats = new vector<Statement*>();
    auto objs = new vector<ASTNode*>();

    while (has_next())
    {
        auto t = next_token();
        unget(t);
        if (t->match(type_start)) {
            int n = decls->size();
            parse_decl(decls, LOCAL_SCOPE);
            for (int i = n;i < (int)(decls->size());++i) {
                auto v = (*decls)[i];
                assert(v != nullptr);
                objs->push_back(v);
            }
        }
        else if (t->match(stat_start)) {
            auto stat = parse_stat(true);
            assert(stat != nullptr);
            stats->push_back(stat);
            objs->push_back(stat);
        }
        else {
            if (!t->match(RBRACE)) {
                show_parser_error(t, "term expected");
            }
            break;
        }
    }
    auto rc = expect_rc(lc);
    auto body = new FuncBody(lc, decls, stats, objs, rc);
    return body;
}

Statement* Parser::parse_stat(bool check)
{
    Statement* stat = nullptr;
    if (check) {
        if (!next_match(stat_start)) {
            show_parser_error(last_token, "term expected");
        }
        else
            unget(last_token);
    }

    if (has_next()) {
        auto t = next_token();
        switch (t->type)
        {
        case SEMICOLON:
            stat = new SemiStatement(t);
            break;
        case BREAK: {
            if (!next_match(SEMICOLON)) {
                ostringstream oss;
                oss << "expect ';'";
                show_parser_error(last_token, oss);
            }
            stat = new BreakStatement(t, last_token);
            break;
        }
        case CONTINUE: {
            if (!next_match(SEMICOLON)) {
                ostringstream oss;
                oss << "expect ';'";
                show_parser_error(last_token, oss);
            }
            stat = new ContinueStatement(t, last_token);
            break;
        }
        case RETURN: {
            Expr* e = nullptr;
            if (next_match(expr_start)) {
                unget(last_token);
                e = parse_expr();
            }
            if (!next_match(SEMICOLON)) {
                ostringstream oss;
                oss << "expect ';'";
                show_parser_error(last_token, oss);
            }
            stat = new ReturnStatement(t, e, last_token);
            break;
        }
        case LBRACE: {
            auto stats = parse_block_body();
            auto rc = expect_rc(t);
            stat = new BlockStatement(t, stats, rc);
            break;
        }
        case IF: {
            stat = parse_if_stat(t);
            break;
        }
        case WHILE: {
            stat = parse_while_stat(t);
            break;
        }
        case DO: {
            stat = parse_do_stat(t);
            break;
        }
        case FOR: {
            stat = parse_for_stat(t);
            break;
        }
        default:
            unget(t);
            if (t->match(expr_start)) {
                auto e = parse_expr();
                expect_semi();
                stat = new ExprStatement(e, last_token);
            }
            break;
        }
    }
    return stat;
}

Statement* Parser::parse_if_stat(const Token* keyword)
{
    Statement* s = nullptr;
    auto lp = expect_lp();
    auto cond = parse_expr();
    auto rp = expect_rp(lp);
    auto stat1 = parse_stat(true);


    if (next_match(ELSE)) {
        auto else_token = last_token;
        auto stat2 = parse_stat(true);
        s = new IfStatement(keyword, lp, cond, rp, stat1, else_token, stat2);
    }
    else {
        s = new IfStatement(keyword, lp, cond, rp, stat1);
    }
    return s;
}

Statement* Parser::parse_while_stat(const Token* keyword)
{
    auto lp = expect_lp();
    auto cond = parse_expr();
    auto rp = expect_rp(lp);
    auto stat = parse_stat(true);

    return new WhileStatement(keyword, lp, cond, rp, stat);
}
Statement* Parser::parse_do_stat(const Token* keyword)
{
    auto stat = parse_stat(true);
    auto while_token = expect(WHILE, "while");
    auto lp = expect_lp();
    auto cond = parse_expr();
    auto rp = expect_rp(lp);
    auto semi = expect_semi();

    return new DoStatement(keyword, stat, while_token, lp, cond, rp, semi);
}

void Parser::update_scope()
{
    for (auto e : *program) {
        e->set_scope(GLOBAL_SCOPE);
    }
}

void Parser::check_semantic()
{
    if (debug_flag[4]) {
        cout << "begin of Parser::check_semantic" << endl;
    }
    update_scope();
    SymbolTableStack stack;
    for (auto e : *program) {
        e->check_semantic(stack);
    }
}

void Parser::print_semantic_info() const
{
    for (auto e : *program) {
        e->print_semantic_info(0);
    }
}

void Parser::pre_gen_code()
{
    for (auto e : *program) {
        auto a = dynamic_cast<ArrayDecl*>(e);
        if (a != nullptr) {
            a->pre_gen_code();
            need_init_fun = true;
            global_symbols.push_back(a);
            continue;
        }
        auto v = dynamic_cast<VarDecl*>(e);
        if (v != nullptr) {
            v->pre_gen_code();
            global_symbols.push_back(v);
            if (v->init_expr != nullptr) {
                //global_init_vars.push_back(v);
                need_init_fun = true;
            }
            continue;
        }
        auto f = dynamic_cast<FunDecl*>(e);
        if (f != nullptr) {
            f->pre_gen_code();
            global_functions.push_back(f);
            continue;
        }
    }
}

void Parser::gen_code()
{
    cginfo = new CgInfo();
    casename = get_case_name(fullinfilenpath);
    jfilename = casename + ".j";
    jout = new ofstream(jfilename);

    auto& jfile = *jout;
    (jfile) << ".class public " << casename << endl;
    (jfile) << ".super java/lang/Object" << endl;
    (jfile) << endl;

    for (auto e : global_symbols) {
        jfile << ".field public static " << e->name->lexme << " " << e->utf_name << endl;
    }

    table_stack = new SymbolTableStack();
    table_stack->pop_back();  //因为我们设计时stack 自带一个table,要先把它弹出来!!

    table_stack->push_back(program);
    assert(table_stack->size() == 1);
    for (auto f : global_functions) {
        if (f->body != nullptr) {
            //cout << "parsing function " <<(f->name->lexme)<< endl;
            assert(table_stack->size() == 1);

            table_stack->current_fun = f;
            table_stack->push_back(&(f->all_vars));
            assert(table_stack->size() == 2);

            f->gen_code(*table_stack);
            table_stack->pop_back();

            assert(table_stack->size() == 1);
            table_stack->current_fun = nullptr;
        }
    }

    (jfile) << endl;
    (jfile) << "; Special methods" << endl;
    (jfile) << endl;

    if (need_init_fun) {
        gen_cl_init();
    }

    gen_final_main();
}

void Parser::gen_cl_init() const
{
    auto jfile = jout;
    (*jfile) << ".method <clinit> : ()V" << endl;
    print_tabs(*jfile, 1);
    (*jfile) << ".code stack 1 locals 0" << endl;

    for (auto e : *program) {
        auto a = dynamic_cast<ArrayDecl*>(e);
        if (a != nullptr) {
            a->gen_init_code();
            continue;
        }
        auto v = dynamic_cast<VarDecl*>(e);
        if (v != nullptr) {
            if (v->init_expr != nullptr) {
                v->gen_init_code(*table_stack);
            }
            continue;
        }
    }

    print_tabs(*jfile);
    (*jfile) << "return" << endl;
    print_tabs(*jfile, 1);
    (*jfile) << ".end code" << endl;
    (*jfile) << ".end method" << endl;
}

void Parser::gen_final_main() const
{
    auto jfile = jout;
    (*jfile) << ".method <init> : ()V" << endl;
    (*jfile) << "    .code stack 2 locals 2" << endl;
    (*jfile) << "        aload_0" << endl;
    (*jfile) << "        invokespecial Method java/lang/Object <init> ()V" << endl;
    (*jfile) << "        return" << endl;
    (*jfile) << "    .end code" << endl;
    (*jfile) << ".end method" << endl;
    (*jfile) << "" << endl;

    if (mode == 5) {
        (*jfile) << ".method public static main : ([Ljava/lang/String;)V" << endl;
        (*jfile) << "    .code stack 2 locals 2" << endl;
        (*jfile) << "        invokestatic Method " << casename << " main ()I" << endl;
        (*jfile) << "        istore_1" << endl;
        (*jfile) << "        getstatic Field java/lang/System out Ljava/io/PrintStream;" << endl;
        (*jfile) << "        ldc 'Return code: '" << endl;
        (*jfile) << "        invokevirtual Method java/io/PrintStream print (Ljava/lang/String;)V" << endl;
        (*jfile) << "        getstatic Field java/lang/System out Ljava/io/PrintStream;" << endl;
        (*jfile) << "        iload_1" << endl;
        (*jfile) << "        invokevirtual Method java/io/PrintStream println (I)V" << endl;
        (*jfile) << "        return" << endl;
        (*jfile) << "    .end code" << endl;
        (*jfile) << ".end method" << endl;
        (*jfile) << "" << endl;
    }
    else {
        (*jfile) << ".method public static main : ([Ljava/lang/String;)V" << endl;
        (*jfile) << "    .code stack 1 locals 1" << endl;
        (*jfile) << "        invokestatic Method " << casename << " main ()I" << endl;
        (*jfile) << "        pop" << endl;
        (*jfile) << "        return" << endl;
        (*jfile) << "    .end code" << endl;
        (*jfile) << ".end method" << endl;
        (*jfile) << "" << endl;
    }
}

Statement* Parser::parse_for_stat(const Token* keyword)
{
    Expr* expr1 = nullptr;
    Expr* expr2 = nullptr;
    Expr* expr3 = nullptr;

    auto lp = expect_lp();
    if (next_match(expr_start)) {
        unget(last_token);
        expr1 = parse_expr();
    }
    auto semi1 = expect_semi();
    if (next_match(expr_start)) {
        unget(last_token);
        expr2 = parse_expr();
    }
    auto semi2 = expect_semi();
    if (next_match(expr_start)) {
        unget(last_token);
        expr3 = parse_expr();
    }
    auto rp = expect_rp(lp);

    auto body = parse_stat(true);
    return new ForStatement(keyword, lp, expr1, semi1, expr2, semi2, expr3, rp, body);
}

vector<Statement*>* Parser::parse_block_body() {
    auto stats = new vector<Statement*>();
    while (has_next()) {
        if (next_match(stat_start)) {
            unget(last_token);
            auto stat = parse_stat(true);
            assert(stat != nullptr);
            stats->push_back(stat);
        }
        else if (next_match(RBRACE)) {
            unget(last_token);
            break;
        }
        else {
            show_parser_error(last_token, "term expected");
        }
    }
    return stats;
}


void build_ecase(const string& filename, const string content) {
    try {
        mode = 6;
        success = true;
        debug_flag[4] = 1;
        debug_flag[5] = 1;
        debug_flag[6] = 1;
        fullinfilenpath = filename;
        init_tokentype_table();
        write_to_file(filename, content);
        Lexer lexer(fullinfilenpath);
        lexer.do_parse();
        assert(success);

        cout << "lexer success" << endl;

        Parser* parser = new Parser();
        parser->do_parse();
        assert(success);

        cout << "parser success" << endl;
        cout << "has " << parser->program->size() << " decls" << endl;

        parser->check_semantic();
        parser->pre_gen_code();
        parser->gen_code();
    }
    catch (bool) {

    }
}

void condition_test1() {
    cout << "case : condition test1" << endl;
    build_ecase("1.txt", "void f(){int x[10]; if (x) x=10;}");
}

void condition_test2() {
    cout << "case : condition test2" << endl;
    build_ecase("1.txt", "struct A{};\n void f(){struct A a; if (1){while(a) ;};}");
}

void condition_test3() {
    cout << "case : condition test3" << endl;
    build_ecase("1.txt", "struct A{int x[20];};\n void f(){struct A a; if (1){for(;a.x;);  }}");
}

void condition_test4() {
    cout << "case : condition test4" << endl;
    build_ecase("1.txt", "struct A{int x[20];}; struct B{struct A b;};\n void f(){struct B x; while (0){\n do{}while(x.b); }}");
}


void compare_test1() {
    cout << "compare_test1()" << endl;
    build_ecase("1.txt", "int x=2<3;int main(putchar()  return 0;)");
}
void part6_test()
{
    //condition_test1();
    //condition_test2();
    //condition_test3();
    //condition_test4();
}
