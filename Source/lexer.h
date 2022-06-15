#pragma once
#include "tools.h"
#include "token.h"
#include "streamstack.h"
#include "ifdirective.h"
// 词法分析器的类
struct Lexer
{

    bool no_errors = true; // 如果词法分析过程中有错,这个变量会被设为false
    bool no_warnings = true;  //如果词法分析过程中有warning,这个变量会被设为false

    Token token; //当前得到的token


    //是否输出part1中的调试信息
    int char_num = 0; //统计一共处理了多少个字符,用于调试

    CharInfo last_pos; //记录最后一个被读取的字符的信息

    //输入流栈
    mutable StreamStack input;



    // 判断词法分析的过程是否成功
    bool success()const {
        return no_warnings && no_errors;
    }

    Lexer(const string& filename);

    //输出错误信息
    void show_error(const CharInfo& pos, const string& des);

    //输出警告信息
    void show_warning(const CharInfo& pos, const string& des);

    // 输出预处理指令的相关错误信息
    void show_directive_error(const CharInfo& pos, const string& des);

    void show_directive_error(const IfDirective& dir, const string& des);

    // 输出关于注释的错误信息
    void show_unclose_comment(const CharInfo& start);

    //判断是否还有待读取的字符对象
    bool has_next()const;

    //读取下一个字符对象
    char get(CharInfo& pos);

    //把一个字符对象回退到缓冲区找那个
    void unget(const CharInfo& pos);

    //词法分析的主函数
    void do_parse();

    // 处理分析到的token
    void process_token();

    // 根据得到的第一个非空白字符进行处理
    void process(const CharInfo& pos);

    // 忽略空白和注释符
    void skip_space_and_comment();

    //忽略行注释
    void process_line_comment(const CharInfo& start);

    //忽略块注释
    void process_block_comment(const CharInfo& start);

    // 处理运算符开头
    void process_op2(char c);

    // 处理id
    void process_id(const CharInfo& start);

    // 处理0开头的情况
    void process_zero(const CharInfo& start);

    //处理8进制数字
    void process_oct_number(const CharInfo& start);

    //处理16进制数字
    void process_hex_number(const CharInfo& start);

    //处理十进制数字(包括类似于0.001的数字)
    void process_dec_number(const CharInfo& start);

    // 处理字符常量
    void process_char_lit(const CharInfo& start);

    // 处理字符串常量
    void process_string_lit(const CharInfo& start);


    // 下面开始的函数和part2有关

    void process_directive(const CharInfo& start);

    string read_directive_name(CharInfo& pos); //获取指令名

    // 读取要包含的文件名
    string read_filename(CharInfo& pos);

    // 处理include指令
    void do_include();


    // 处理define指令
    void do_define();

    // 处理undef指令
    void do_undef();

    // 读取macro的name
    string read_macro_name(CharInfo& pos);

    string read_macro_body();

    // 处理 #ifdef
    void do_ifdef(const CharInfo& start);

    // 处理 #ifudef
    void do_ifndef(const CharInfo& start);

    // 处理 #else
    void do_pound_else(const CharInfo& start);

    // 处理 #endif
    void do_endif(const CharInfo& start);


    //忽略#和预处理指令之间的空白符和块注释
    void skip_space_and_block_comment();

    // 确定当前如果有条件编译指令时,本分支的值是否为true
    // 为真时要展开宏,把得到的token插入到词法流序列的结尾
    bool need_process_token()const;
};

