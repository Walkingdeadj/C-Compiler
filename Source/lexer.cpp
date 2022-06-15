#include "lexer.h"

#include "macro.h"
#include "ifdirective.h"
#include "lexer.h"

const int MAX_ID_LENGTH = 48;
const int MAX_INT_LENGTH = 48;
const int MAX_REAL_LENGTH = 48;
const int MAX_STRING_LENGTH = 1024;

bool Lexer::has_next() const {
    return input.has_next();
}

void Lexer::show_error(const CharInfo& pos, const string& des) {
    no_errors = false;
    cerr << "Lexer error in file "
        << pos.stream->filename
        << " line "
        << pos.line_no
        ;

    cerr << "\n";

    cerr << '\t' << des << "\n";
    exit(1);
}

void Lexer::show_warning(const CharInfo& pos, const string& des) {
    no_warnings = false;
    cerr << "Lexer warning in file "
        << pos.stream->filename
        << " line "
        << pos.line_no
        ;

    cerr << endl;

    cerr << '\t' << des << endl;
    exit(1);
}

void Lexer::show_directive_error(const CharInfo& pos, const string& des)
{
    no_errors = false;
    cerr << "Preprocessor error in file "
        << pos.stream->filename
        << " line "
        << pos.line_no
        ;

    cerr << "\n";

    cerr << '\t' << des << "\n";
    exit(1);
}

void Lexer::show_directive_error(const IfDirective& dir, const string& des)
{
    no_errors = false;
    cerr << "Preprocessor error in file "
        << dir.filename
        << " line "
        << dir.line_no
        ;

    cerr << "\n";

    cerr << '\t' << des << "\n";
    exit(1);
}



char Lexer::get(CharInfo& pos)
{
    input.pop(pos);
    ++char_num;
    last_pos = pos;
    return pos.value;
}

void Lexer::unget(const CharInfo& pos)
{
    --char_num;
    pos.stream->unget(pos);
}

Lexer::Lexer(const string& filename) {
    input.push(filename);
}

// 根据读取到的第一个非空白字符进行后续处理
void Lexer::process(const CharInfo& start)
{
    char c = start.value;
    token.pos = start;
    token.lexme = string(1, c);
    token.type = UNKNOWN;

    if (c == '_' || isalpha(c)) {
        process_id(start);
    }
    else if (isdigit(c)) {
        if (c == '0')
            process_zero(start);
        else
            process_dec_number(start);
    }
    else if (is_op2_start(c)) {
        process_op2(c);
    }
    else if (is_op1(c)) {
        token.type = get_op1_type(c);
        process_token();
    }
    else if (c == '\"') {
        process_string_lit(start);
    }
    else if (c == '\'') {
        process_char_lit(start);
    }
    else if (c == '#') {
        process_directive(start);
    }
    else {
        if (need_process_token()) {
            string des = "Ignoring unexpected symbol: ";
            des += c;
            show_error(start, des);
        }
    }
}

void Lexer::do_parse()
{
    while (has_next())
    {
        skip_space_and_comment();

        if (has_next()) {
            CharInfo pos;
            get(pos);

            if (debug_flag[1]) {
                cout << "to process char " << pos.value << endl;
            }

            process(pos);
        }
        else
            break;
    }

    if (!if_dire_stack.empty()) {
        auto s = if_dire_stack.top();
        ostringstream oss;
        if (s->in_else_branch)
            oss << "#else on line " << s->else_line_no << " unclosed";
        else
            oss << "#" << s->dir_name << " on line " << s->line_no << " unclosed";
        show_directive_error(*s, oss.str());
    }
}

void Lexer::skip_space_and_comment()
{
    while (has_next()) {
        CharInfo pos;
        char c = get(pos);

        bool stopped = true; //遇到非空白和非注释则停止循环
        if (isspace(c))
            stopped = false;
        else if (c == '/') {
            if (has_next()) {
                CharInfo pos2;
                char c2 = get(pos2);
                if (c2 == '/') {
                    stopped = false;
                    process_line_comment(pos);
                }
                else if (c2 == '*') {
                    stopped = false;
                    process_block_comment(pos);
                }
                else {
                    unget(pos2);
                }
            }
        }

        if (stopped) {
            unget(pos);
            break;
        }
    }
}

bool is_valid_directive(const string& s) {
    return s == "include"
        || s == "define"
        || s == "undef"
        || s == "ifdef"
        || s == "ifndef"
        || s == "else"
        || s == "endif"
        ;
}

void Lexer::process_directive(const CharInfo& start)
{
    CharInfo pos;
    string name = read_directive_name(pos);

    if (!is_valid_directive(name)) {
        string des = "mistake directive name ";
        des += name;
        show_directive_error(pos, des);
    }

    if (name == "include") {
        do_include();
    }
    else if (name == "define") {
        do_define();
    }
    else if (name == "undef") {
        do_undef();
    }
    else if (name == "ifdef") {
        do_ifdef(pos);
    }
    else if (name == "ifndef") {
        do_ifndef(pos);
    }
    else if (name == "else") {
        do_pound_else(pos);
    }
    else if (name == "endif") {
        do_endif(pos);
    }
}

string Lexer::read_directive_name(CharInfo& start)
{
    skip_space_and_block_comment();
    if (!has_next()) {
        string des = "empty directive name";
        show_directive_error(last_pos, des);
    }

    string name;
    get(start);
    unget(start);

    while (has_next()) {
        CharInfo pos;
        char c = get(pos);
        if (isalpha(c))
            name += c;
        else {
            unget(pos);
            break;
        }
    }

    if (debug_flag[2])
        cout << "directive_name is: " << name << endl;

    return name;
}

string Lexer::read_filename(CharInfo& start)
{
    skip_space_and_block_comment();
    if (!has_next()) {
        ostringstream oss;
        oss << "need filename after include directive";
        show_directive_error(last_pos, oss.str());
    }

    string s;
    char c = get(start);
    if (c != '\"') {
        ostringstream oss;
        oss << "filename must be in quote";
        show_directive_error(start, oss.str());
    }

    while (has_next()) {
        CharInfo pos;
        c = get(pos);
        if (c == '\"')
            break;
        else
            s += c;
    }

    if (debug_flag[2]) {
        cout << "filename is " << s << endl;
    }

    return s;
}

void Lexer::do_include()
{
    CharInfo pos;
    string filename = read_filename(pos);

    if (filename.empty() || !file_exists(filename)) {
        string des = "file not exist:";
        des += filename;
        show_directive_error(pos, des);
    }

    if (pos.stream->has_include(filename)) {
        ostringstream oss;
        oss << "cycle include file " << filename;
        show_directive_error(pos, oss.str());
    }

    pos.print_info();
    cout << " include expansion" << endl;

    input.push(filename, pos.stream);
}

void Lexer::do_define()
{
    CharInfo pos;
    string name = read_macro_name(pos);

    if (is_macro_exists(name)) {
        ostringstream oss;
        oss << "macro " << name << "has been exist";
        show_directive_error(pos, oss.str());
    }

    string s = read_macro_body();
    macro_table[name] = s;
}

void Lexer::do_undef()
{
    CharInfo pos;
    string name = read_macro_name(pos);

    if (is_macro_exists(name)) {
        macro_table.erase(name);

        if (debug_flag[2])
            cout << "macro " << name << " has been removed" << endl;
    }
}

string Lexer::read_macro_name(CharInfo& start)
{
    skip_space_and_block_comment();

    if (!has_next()) {
        string des = "empty macro name";
        show_directive_error(last_pos, des);
    }

    string name;
    char c = get(start);
    if (!(isalpha(c) || c == '_')) {
        string des = "macro name must be an id";
        show_directive_error(start, des);
    }

    name += c;
    while (has_next()) {
        CharInfo pos;
        c = get(pos);
        if (isalnum(c) || c == '_')
            name += c;
        else {
            unget(pos);
            break;
        }
    }

    if (debug_flag[2])
        cout << "macro_name is: " << name << endl;
    return name;
}

string  Lexer::read_macro_body()
{
    skip_space_and_block_comment();
    string s;
    while (has_next())
    {
        CharInfo pos;
        char c = get(pos);
        if (c == '\n')
            break;
        else
            s += c;
    }

    if (debug_flag[2])
        cout << "macro body is: " << s << endl;

    return s;
}

void Lexer::do_ifdef(const CharInfo& start)
{
    CharInfo pos;
    string name = read_macro_name(pos);

    IfDirective* p = nullptr;
    if (if_dire_stack.empty())
        p = new IfDirective(start.stream->filename
            , start.line_no
            , name, "ifdef");
    else
        p = new IfDirective(start.stream->filename
            , start.line_no,
            name, "ifdef", if_dire_stack.top());

    if_dire_stack.push(p);
}

void Lexer::do_ifndef(const CharInfo& start)
{
    CharInfo pos;
    string name = read_macro_name(pos);

    IfDirective* p = nullptr;
    if (if_dire_stack.empty())
        p = new IfDirective(start.stream->filename
            , start.line_no
            , name, "ifndef");
    else
        p = new IfDirective(start.stream->filename
            , start.line_no,
            name, "ifndef", if_dire_stack.top());

    if_dire_stack.push(p);
}

void Lexer::do_pound_else(const CharInfo& pos)
{
    if (if_dire_stack.empty()) {
        ostringstream oss;
        oss << "#else without #if, ignoring";
        show_directive_error(pos, oss.str());
    }

    auto s = if_dire_stack.top();
    if (s->in_else_branch) {
        ostringstream oss;
        oss << "second #else (first on line " << s->else_line_no << "), ignoring";
        show_directive_error(pos, oss.str());
    }

    s->else_line_no = pos.line_no;
    s->in_else_branch = true;
}

void Lexer::do_endif(const CharInfo& pos)
{
    if (if_dire_stack.empty()) {
        ostringstream oss;
        oss << "#endif without #if, ignoring";
        show_directive_error(pos, oss.str());
    }

    if_dire_stack.pop();
}

//从当前字符开始,忽略连续的空白(回车符除外)和块注释
void Lexer::skip_space_and_block_comment() {
    while (has_next()) {
        CharInfo pos;
        char c = get(pos);

        bool stopped = false; //遇到非空白和非注释则停止循环
        if (c == '\n')
            stopped = true;
        else if (isspace(c)) {
            stopped = false;
        }
        else {
            stopped = true;
            if (c == '/' && has_next()) {
                CharInfo pos2;
                char c2 = get(pos2);
                if (c2 == '*') {
                    stopped = false;
                    process_block_comment(pos);
                }
                else {
                    unget(pos2);
                }
            }
        }
        if (stopped) {
            unget(pos);
            break;
        }
    }
}

// 输出非封闭注释的错误提示
void Lexer::show_unclose_comment(const CharInfo& start) {
    show_error(start, "Unclosed comment.");
}

//忽略行注释
void Lexer::process_line_comment(const CharInfo& start)
{
    // 记录注释是否封闭
    bool closed = false;
    while (has_next())
    {
        CharInfo pos;
        char c = get(pos);
        if (c == '\n') {
            closed = true;
            break;
        }
    }

    if (debug_flag[1]) {
        cout << "end of line comment " << endl;
    }

    if (!closed)
        show_unclose_comment(start);
}

//忽略块注释
void Lexer::process_block_comment(const CharInfo& start)
{
    // 注释是否封闭
    bool closed = false;
    char last_char = 0;  //记录上次读取的字符
    while (has_next()) {
        CharInfo pos;
        char c = get(pos);

        // 遇到 */ 即认为结束
        if (c == '/' && last_char == '*') {
            closed = true;
            break;
        }

        last_char = c;
    }


    if (debug_flag[1]) {
        cout << "end of block comment " << endl;
    }

    if (!closed)
        show_unclose_comment(start);
}

void Lexer::process_op2(char c)
{
    bool op2_flag = false;
    if (has_next()) {
        CharInfo pos;
        get(pos);
        string s(1, c);
        s += pos.value;
        if (is_op2(s)) {
            op2_flag = true;
            token.type = get_op2_type(s);
            token.lexme = s;
        }
        else {
            unget(pos);
        }
    }
    if (!op2_flag) {
        token.type = get_op1_type(c);
    }

    process_token();
}

//处理id的其余字符
void Lexer::process_id(const CharInfo& start)
{
    string& s = token.lexme;
    while (has_next()) {
        CharInfo pos;
        char c = get(pos);
        if (isalnum(c) || c == '_') {
            s += c;
        }
        else {
            unget(pos);
            break;
        }
    }

    if (s.size() > MAX_ID_LENGTH) {
        string des = "Identifier too long; truncating to ";
        des += s.substr(0, MAX_ID_LENGTH);
        show_warning(start, des);
    }

    if (is_macro_exists(s)) {
        if (start.stream->has_expand(s)) {
            ostringstream oss;
            oss << "cycle expand macro " << s;
            show_directive_error(start, oss.str());
        }
        // 进行宏展开
        start.print_info();
        cout << " macro expansion" << endl;
        input.push(s, start.line_no, start.stream);
    }
    else {
        token.type = ID;
        if (is_type_keyword(s))
            token.type = TYPE;
        else if (is_keyword(s))
            token.type = get_keyword_type(s);
        process_token();
    }
}

void Lexer::process_zero(const CharInfo& start)
{
    token.type = INT_LIT;
    string& s = token.lexme;
    int base = 8;  //默认为8进制

    if (has_next()) {
        CharInfo pos;
        char c = get(pos);
        if (tolower(c) == 'x') {
            base = 16;   //0x和0X代表16进制
            s += c;
        }
        else if (c == '.') {
            unget(pos);
            process_dec_number(start);
            return;
        }
        else {
            unget(pos);
        }
    }

    if (base == 8) {
        process_oct_number(start);
    }
    else {
        process_hex_number(start);
    }
}

void Lexer::process_oct_number(const CharInfo& start)
{
    string& s = token.lexme;
    bool success = true;
    while (has_next()) {
        CharInfo pos;
        char c = get(pos);

        if (c >= '0' && c <= '7') {
            s += c;
        }
        else if (isxdigit(c)) {
            success = false;
            s += c;
            string des = "Invalid digit in octal constant ";
            des += s;
            show_error(start, des);
            break;
        }
        else {
            unget(pos);
            break;
        }
    }

    if (success) {
        if (s.size() > MAX_INT_LENGTH) {
            string des = "Number too long; truncating to ";
            des += s.substr(0, MAX_INT_LENGTH);
            show_warning(start, des);
        }
        process_token();
    }
}

void Lexer::process_hex_number(const CharInfo& start)
{
    string& s = token.lexme;
    bool success = true;
    while (has_next()) {
        CharInfo pos;
        char c = get(pos);
        if (isxdigit(c)) {
            s += c;
        }
        else {
            unget(pos);
            break;
        }
    }

    if (success) {
        if (s.size() > MAX_INT_LENGTH) {
            string des = "Number too long; truncating to ";
            des += s.substr(0, MAX_INT_LENGTH);
            show_warning(start, des);
        }
        process_token();
    }
}

void Lexer::process_dec_number(const CharInfo& start)
{
    string& s = token.lexme;

    bool is_real = false;
    token.type = INT_LIT;
    while (has_next()) {
        CharInfo pos;
        char c = get(pos);
        if (isdigit(c)) {
            s += c;
        }
        else {
            if (c == '.') {
                s += c;
                is_real = true;
            }
            else {
                unget(pos);
            }
            break;
        }
    }

    if (!is_real) {
        if (s.size() > MAX_INT_LENGTH) {
            string des = "Number too long; truncating to ";
            des += s.substr(0, MAX_INT_LENGTH);
            show_warning(start, des);
        }
        process_token();
        return;
    }

    //读取小数部分
    token.type = REAL_LIT;
    bool has_epart = false;
    while (has_next()) {
        CharInfo pos;
        char c = get(pos);
        if (isdigit(c)) {
            s += c;
        }
        else {
            if (toupper(c) == 'E') {
                s += c;
                has_epart = true;
            }
            else {
                unget(pos);
            }
            break;
        }
    }

    //读取指数部分
    if (has_epart) {
        if (has_next()) {
            CharInfo pos;
            char c = get(pos);
            //指数部分的开头可以带一个+或者-
            if (c == '+' || c == '-' || isdigit(c)) {
                s += c;

                //继续读取指数里的数字部分
                while (has_next()) {
                    CharInfo pos2;
                    char c2 = get(pos2);
                    if (isdigit(c2)) {
                        s += c2;
                    }
                    else {
                        unget(pos2);
                        break;
                    }
                }
            }
            else {
                unget(pos);
            }
        }
    }

    if (s.size() > MAX_REAL_LENGTH) {
        string des = "Number too long; truncating to ";
        des += s.substr(0, MAX_REAL_LENGTH);
        show_warning(start, des);
    }
    process_token();
}

void Lexer::process_char_lit(const CharInfo& start)
{
    token.type = CHAR_LIT;
    string& s = token.lexme;

    bool closed = false;
    if (has_next()) {
        CharInfo pos;
        char c = get(pos);
        s += c;
        if (c == '\\') {
            bool escape_flag = false;
            if (has_next()) {
                CharInfo pos2;
                char c2 = get(pos2);
                s += c2;
                if (is_escape_char(c2)) {
                    escape_flag = true;
                }
            }
            if (!escape_flag) {
                show_warning(pos, "unrecorgnized escape sequence");
            }
        }
        if (has_next()) {
            CharInfo pos2;
            char c2 = get(pos2);
            s += c2;
            if (c2 == '\'') {
                closed = true;
            }
        }
    }


    if (!closed) {
        show_error(start, "Unclosed character literal");
    }
    else {
        process_token();
    }
}

void Lexer::process_string_lit(const CharInfo& start)
{
    token.type = STRING_LIT;
    string& s = token.lexme;
    bool closed = false;
    while (has_next()) {
        CharInfo pos;
        char c = get(pos);
        s += c;
        if (c == '\"') {
            closed = true;
            break;
        }
        else if (c == '\n') {
            break;
        }
        else if (c == '\\') {
            bool escape_flag = false;
            if (has_next()) {
                CharInfo pos2;
                char c2 = get(pos2);
                s += c2;
                if (is_escape_char(c2)) {
                    escape_flag = true;
                }
            }
            if (!escape_flag) {
                show_warning(pos, "unrecorgnized escape sequence");
            }
        }
    }

    if (!closed) {
        show_error(start, "Unclosed string literal");
    }

    if (s.size() > MAX_STRING_LENGTH) {
        string des = "string literal too long; truncating to ";
        des += s.substr(0, MAX_STRING_LENGTH);
        show_warning(start, des);
    }

    process_token();
}

bool Lexer::need_process_token() const
{
    return if_dire_stack.empty() || if_dire_stack.top()->need_expand();
}

//处理得到的新的token
void Lexer::process_token() {
    if (need_process_token()) {
        // 在正常状态下,得到的token的类型必须是一个合法的已知值
        assert(token.type != UNKNOWN);

        if (mode == 1 || mode == 2) {
            cout << token << endl;
        }
        tokens.push_back(token); //插入新的词法对象
    }
}