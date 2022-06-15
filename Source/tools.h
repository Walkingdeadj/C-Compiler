#pragma once
#pragma once
// 引入一些常见的c++头文件
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <stack>
#include <list>
#include <set>
#include <map>
#include <queue>
#include <memory>
#include <iterator>
#include <exception>

// 引入C的常见头文件
#include <cassert>
#include <cstdlib>
#include <cctype>
using namespace std;

//全局变量的定义
extern int mode;  //当前所处的模式
extern int debug_flag[10]; //各作业的测试标志
extern string fullinfilenpath;
extern string outfilepath;
extern ofstream* ofs;
extern bool success;  //是否运行成功
extern string casename; //用例名,即去掉扩展名之后的文件名.比如输入文件的完整的路径为1.c,那么casename就是1
extern string jfilename; //输出的j代码文件名,在part5和part6中使用

extern ofstream* jout;

// 一些常见的工具函数

// 判断文件是否存在
bool file_exists(const char* filename);
bool file_exists(const string& filename);

//获取用例名
string get_case_name(const string& inputname);

bool is_escape_char(char c);

//文件中写入内容,可用于构造自己的测试用例
void write_to_file(const string& filename, const string& content = "");

//输出tab,用于代码生成
void print_tabs(ostream& os, int n = 2);

// 将字符串转换为整数
int my_atoi(const string& t);

//生成类型转换的汇编代码
void gen_convert_inst(ostream& os, const string& res_type, const string& act_type);

void show_help();
void show_version();
void set_mode(const string& arg);

void set_ofs(int argc, char* argv[]);
void set_infile(int argc, char* argv[]);

//输出单条指令
void print_inst(const string&s);

//将栈顶结果+1再入栈
void gen_inc(const string& type);

//将栈顶结果-1再入栈
void gen_dec(const string& type);

//将常数1入栈
void print_const1(const string& type);

void print_comment(const string&s);


void push_true();
void push_false();

void print_label(int n);
void print_goto(int n);

string high_type(const string&A, const string& B);
