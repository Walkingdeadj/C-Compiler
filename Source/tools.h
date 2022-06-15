#pragma once
#pragma once
// ����һЩ������c++ͷ�ļ�
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

// ����C�ĳ���ͷ�ļ�
#include <cassert>
#include <cstdlib>
#include <cctype>
using namespace std;

//ȫ�ֱ����Ķ���
extern int mode;  //��ǰ������ģʽ
extern int debug_flag[10]; //����ҵ�Ĳ��Ա�־
extern string fullinfilenpath;
extern string outfilepath;
extern ofstream* ofs;
extern bool success;  //�Ƿ����гɹ�
extern string casename; //������,��ȥ����չ��֮����ļ���.���������ļ���������·��Ϊ1.c,��ôcasename����1
extern string jfilename; //�����j�����ļ���,��part5��part6��ʹ��

extern ofstream* jout;

// һЩ�����Ĺ��ߺ���

// �ж��ļ��Ƿ����
bool file_exists(const char* filename);
bool file_exists(const string& filename);

//��ȡ������
string get_case_name(const string& inputname);

bool is_escape_char(char c);

//�ļ���д������,�����ڹ����Լ��Ĳ�������
void write_to_file(const string& filename, const string& content = "");

//���tab,���ڴ�������
void print_tabs(ostream& os, int n = 2);

// ���ַ���ת��Ϊ����
int my_atoi(const string& t);

//��������ת���Ļ�����
void gen_convert_inst(ostream& os, const string& res_type, const string& act_type);

void show_help();
void show_version();
void set_mode(const string& arg);

void set_ofs(int argc, char* argv[]);
void set_infile(int argc, char* argv[]);

//�������ָ��
void print_inst(const string&s);

//��ջ�����+1����ջ
void gen_inc(const string& type);

//��ջ�����-1����ջ
void gen_dec(const string& type);

//������1��ջ
void print_const1(const string& type);

void print_comment(const string&s);


void push_true();
void push_false();

void print_label(int n);
void print_goto(int n);

string high_type(const string&A, const string& B);
