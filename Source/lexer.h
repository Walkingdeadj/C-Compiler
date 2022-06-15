#pragma once
#include "tools.h"
#include "token.h"
#include "streamstack.h"
#include "ifdirective.h"
// �ʷ�����������
struct Lexer
{

    bool no_errors = true; // ����ʷ������������д�,��������ᱻ��Ϊfalse
    bool no_warnings = true;  //����ʷ�������������warning,��������ᱻ��Ϊfalse

    Token token; //��ǰ�õ���token


    //�Ƿ����part1�еĵ�����Ϣ
    int char_num = 0; //ͳ��һ�������˶��ٸ��ַ�,���ڵ���

    CharInfo last_pos; //��¼���һ������ȡ���ַ�����Ϣ

    //������ջ
    mutable StreamStack input;



    // �жϴʷ������Ĺ����Ƿ�ɹ�
    bool success()const {
        return no_warnings && no_errors;
    }

    Lexer(const string& filename);

    //���������Ϣ
    void show_error(const CharInfo& pos, const string& des);

    //���������Ϣ
    void show_warning(const CharInfo& pos, const string& des);

    // ���Ԥ����ָ�����ش�����Ϣ
    void show_directive_error(const CharInfo& pos, const string& des);

    void show_directive_error(const IfDirective& dir, const string& des);

    // �������ע�͵Ĵ�����Ϣ
    void show_unclose_comment(const CharInfo& start);

    //�ж��Ƿ��д���ȡ���ַ�����
    bool has_next()const;

    //��ȡ��һ���ַ�����
    char get(CharInfo& pos);

    //��һ���ַ�������˵����������Ǹ�
    void unget(const CharInfo& pos);

    //�ʷ�������������
    void do_parse();

    // �����������token
    void process_token();

    // ���ݵõ��ĵ�һ���ǿհ��ַ����д���
    void process(const CharInfo& pos);

    // ���Կհ׺�ע�ͷ�
    void skip_space_and_comment();

    //������ע��
    void process_line_comment(const CharInfo& start);

    //���Կ�ע��
    void process_block_comment(const CharInfo& start);

    // �����������ͷ
    void process_op2(char c);

    // ����id
    void process_id(const CharInfo& start);

    // ����0��ͷ�����
    void process_zero(const CharInfo& start);

    //����8��������
    void process_oct_number(const CharInfo& start);

    //����16��������
    void process_hex_number(const CharInfo& start);

    //����ʮ��������(����������0.001������)
    void process_dec_number(const CharInfo& start);

    // �����ַ�����
    void process_char_lit(const CharInfo& start);

    // �����ַ�������
    void process_string_lit(const CharInfo& start);


    // ���濪ʼ�ĺ�����part2�й�

    void process_directive(const CharInfo& start);

    string read_directive_name(CharInfo& pos); //��ȡָ����

    // ��ȡҪ�������ļ���
    string read_filename(CharInfo& pos);

    // ����includeָ��
    void do_include();


    // ����defineָ��
    void do_define();

    // ����undefָ��
    void do_undef();

    // ��ȡmacro��name
    string read_macro_name(CharInfo& pos);

    string read_macro_body();

    // ���� #ifdef
    void do_ifdef(const CharInfo& start);

    // ���� #ifudef
    void do_ifndef(const CharInfo& start);

    // ���� #else
    void do_pound_else(const CharInfo& start);

    // ���� #endif
    void do_endif(const CharInfo& start);


    //����#��Ԥ����ָ��֮��Ŀհ׷��Ϳ�ע��
    void skip_space_and_block_comment();

    // ȷ����ǰ�������������ָ��ʱ,����֧��ֵ�Ƿ�Ϊtrue
    // Ϊ��ʱҪչ����,�ѵõ���token���뵽�ʷ������еĽ�β
    bool need_process_token()const;
};

