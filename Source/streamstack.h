#pragma once
#include "tools.h"
#include "charstream.h"
/*
��������ɵ�ջ,��part1,���ջ��ֻ��һ��������
,��part2,������include�����ߺ�չ��ʱ,���
�µ�����������ŵ�ջ��,ÿ�ζ��ȴ�ջ���ж�ȡ�ַ�
lexer�����ջ���н���.
��һ���������Ѿ������βʱ,�������ϰ���ջ���Ƴ�.
ͬ��,ֻ�зǿյ��������������ջ.
*/
struct StreamStack {
    stack<CharStream*> streams;

    char pop(CharInfo& pos);  // ��ȡ��һ���ַ��Ѿ�����Դ��Ϣ
    bool has_next(); //�ж��Ƿ���δ���ַ�,����Ѿ��������Ľ�β�򷵻�false

    void clean();  //������Ϊ�յ�����������ջ��

    // ���붥��Ĵ����ļ�,ֻ������һ��
    void push(const string& filename);

    /*
     * ����һ���ļ�����������ջ
     * �����߱��뱣֤�ļ����Ѿ�����
     * ���������ڲ��뱻include���ļ�
     * ������part2
     */
    void push(const string& filename, CharStream* father);

    /*
     * ����һ����չ��������������ջ
     * ������part2
     */
    void push(const string& macroname, int line_no, CharStream* father);
};