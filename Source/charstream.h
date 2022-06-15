#pragma once
#include "tools.h"
#include "macro.h"

// �����ַ�����ʵ��
// �������ݿ�����Դ���ļ�
// Ҳ������Դ���ַ���

#include "charinfo.h"

struct CharStream
{
    CharInfo pos; //��һ������ȡ�ַ���λ����Ϣ

    //���ڻ��˵Ļ�����,
    stack<CharInfo> buffer;

    //�ж�������Դ�Ƿ�Ϊ��չ��
    bool is_macro_expand()const {
        return !macroname.empty();
    }

    // ���ݳ�Ա
    int level = 0;  //����ǵ��������
    string filename = ""; // ��������ص��ļ���,��part2,������������Ժ�չ��,������кŴ����չ��ʱ���ڵ��ļ���
    string macroname = ""; //��������صĺ���,Ĭ��Ϊ�մ�

    istream* is = nullptr;  //�����������ָ��,������Դ���ļ�,������Դ���ַ���

    CharStream* father = nullptr; //����ָ��,�����ж��ļ�Ƕ�׺ͺ�չ���Ƿ���ڻ�·.

    //�����ǳ�Ա����
    CharStream(const string& filename)
        :filename(filename), is(new ifstream(filename))
    {

    }

    CharStream(const string& filename, CharStream* father)
        :level(father->level + 1)
        , filename(filename)
        , is(new ifstream(filename))
        , father(father)
    {

    }

    CharStream(const string& macroname, int line_no, CharStream* father)
        :level(father->level + 1)
        , filename(father->filename)
        , macroname(macroname)
        , is(new istringstream(get_macro_body(macroname)))
        , father(father)
    {
        if (debug_flag[2])
            cout << "push macro stream with name " << macroname
            << " in file " << filename
            << " at line " << line_no
            << endl;
        pos.line_no = line_no;
    }

    virtual ~CharStream() {
        delete is;
    }

    // �����кź��к�,����part1����
    void update_position(char c);

    void unget(const CharInfo& pos) {
        if (debug_flag[1]) {
            cout << "unget char info to buffer:";
            cout << pos << endl;
        }

        buffer.push(pos);
    }

    char pop(CharInfo& pos);  // ��ȡ��һ���ַ��Լ���λ����Ϣ
    bool has_next()const; //�ж��Ƿ���δ���ַ�,����Ѿ��������Ľ�β�򷵻�false

    //�ж�ĳ���ļ��Ƿ�չ����,���ڷ�ֹ�ļ��԰���
    bool has_include(const string& filename)const;

    //�ж�ĳ�����Ƿ��Ѿ���չ����,���ڷ�ֹ����Եݹ�չ��
    bool has_expand(const string& macroname)const;

};


//һЩ���Ժ���
void char_stream_test1();
void char_stream_test2();
