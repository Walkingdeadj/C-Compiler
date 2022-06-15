#pragma once
#include "tools.h"
//�ַ�����ľ�����Ϣ,��������,��Դ,λ�õ�
struct CharStream;
struct CharInfo
{
    //�ַ����кź��к�,����part1����Ч
    int line_no = 1; //��part2, ������������Ժ�չ��, ������кŴ����չ������ʱ���к�
    int col_no = 1;

    char value = 0;  //�ַ��ľ���ֵ

    mutable CharStream* stream = nullptr; //��¼���ַ���Դ���ĸ�������

    //�޲ι��캯��
    CharInfo() {

    }

    //�������캯��
    CharInfo(const CharInfo& other) :
        line_no(other.line_no)
        , col_no(other.col_no)
        , value(other.value)
        , stream(other.stream)
    {

    }

    CharInfo& operator=(const CharInfo& other) {
        if (this != &other) {
            this->line_no = other.line_no;
            this->col_no = other.col_no;
            this->value = other.value;
            this->stream = other.stream;
        }
        return *this;
    }

    void print(ostream& os = cout)const;

    // ֻ�����Դ��Ϣ
    void print_info(ostream& os = cout)const;

};

ostream& operator<<(ostream& os, const CharInfo& pos);

