#include "tools.h"


int mode=0;  //当前所处的模式
int debug_flag[10]={0}; //各作业的测试标志,用于手工调试
string fullinfilenpath="";
string outfilepath = "";
ofstream* ofs=nullptr;
bool success=true;  //是否运行成功
string casename=""; //用例名,即去掉扩展名之后的文件名.比如输入文件的完整的路径为1.c,那么casename就是1
string jfilename=""; //输出的j代码文件名,在part5和part6中使用

ofstream* jout =nullptr; 

bool file_exists(const char* filename)
{
    ifstream ifs(filename);
    return ifs.good();
}

bool file_exists(const string& filename)
{
    ifstream ifs(filename);
    return ifs.good();
}

bool is_escape_char(char c2)
{
    return c2 == 'n' || c2 == 'r' || c2 == 'a' || c2 == 'b' || c2 == 't'
        || c2 == '\\' || c2 == '\'' || c2 == '\"'
        ;
}

//文件中写入内容,可用于构造自己的测试用例
void write_to_file(const string& filename, const string& content) {
    ofstream ofs(filename);
    ofs << content;
}

string get_case_name(const string& inputname)
{
    auto pos = inputname.find('.');
    return inputname.substr(0, pos);
}

int my_atoi(const string& s)
{
    istringstream iss(s);
    int x = 0;
    iss >> x;
    return x;
}

void print_tabs(ostream& os, int n)
{
    assert(n <= 2);
    os << string(n, '\t');
}

void gen_convert_inst(ostream& os, const string& res_type, const string& act_type)
{
    if (res_type == "float") {
        if (act_type!=res_type){
            print_tabs(os);
            os << "i2f" << endl;
        }
    }
    else if (res_type == "int" && act_type == "float") {
        print_tabs(os);
        os << "f2i" << endl;
    }
}

void show_help() {
    printf("Usage:\n\tmycc -mode [options] infile \n\n");
    printf("Valid modes: \n\t");
    printf("-0: Version information only\n\t");
    printf("-1: Part 1 C Lexer\n\t");
    printf("-2: Part 2 C Preprocessor \n\t");
    printf("-3: Part 3 C parser \n\t");
    printf("-4: Part 4 Type Checking\n\t");
    printf("-5: Part 5 Code Generation: Expressions\n\n");
    printf("-6: Part 6 Code Generation: Control Flow\n\n");
    printf("Valid options: \n\t");
    printf("-o outfile: write to outfile instead of standard output\n");
}

void show_version() {
    cout << "My C compiler (for COMS 440/450)" << endl;
    cout << "Written by Jian Shi (@iastate.edu)" << endl;
    cout << "Version 6.0" << endl;
    cout << "6 May, 2022" << endl;;
}

void set_mode(const string& arg) {
    if (arg.size() != 2) {
        show_help();
        exit(0);
    }

    char c = arg[1];
    if (arg[0] != '-' || c < '0' || c>'6') {
        show_help();
        exit(0);
    }

    mode = c - '0';
}

// 处理 -o 选项
void set_ofs(int argc, char* argv[]) {
    if (argc >= 3) {
        const string arg = argv[2];
        if (arg == "-o") {
            if (argc < 4)
            {
                show_help();
                exit(0);
            }
            else {
                ofs = new ofstream(argv[3]);
                cout.rdbuf(ofs->rdbuf());
            }
        }
    }
}

void set_infile(int argc, char* argv[]) {
    if (ofs == nullptr) {
        if (argc < 3) {
            show_help();
            exit(0);
        }
        fullinfilenpath = argv[2];
    }
    else {
        if (argc < 5) {
            show_help();
            exit(0);
        }
        fullinfilenpath = argv[4];
    }
}

void print_inst(const string& s)
{
    auto& os =*jout;
    print_tabs(os);
    os<<s<<endl;
}

//将常数1入栈
void print_const1(const string& type) {
    if (type == "float") {
        print_inst("fconst_1");
    }
    else {
        print_inst("iconst_1");
    }
}

void print_comment(const string& s)
{
    auto& os = *jout;
    print_tabs(os, 2);
    os << ";" << s << endl;
}

void push_true()
{
    print_inst("bipush 1");
}

void push_false()
{
    print_inst("bipush 0");
}

void print_goto(int n) {
    auto& os = *jout;
    print_tabs(os, 2);
    os << "goto L" << n << endl;
}

void print_label(int n)
{
    auto& os =*jout;
    print_tabs(os,1);
    os<<"L"<<n<<":" <<endl;
}

string high_type(const string& A, const string& B)
{
    if (A=="float"||B=="float")
        return "float";
    else
        return "int";
}

void gen_inc(const string& type)
{
    print_const1(type);

    if (type == "float") {
        print_inst("fadd");
    }
    else {
        print_inst("iadd");
    }
}

void gen_dec(const string& type)
{
    print_const1(type);
    if (type == "float") {
        print_inst("fsub");
    }
    else {
        print_inst("isub");
    }
}






