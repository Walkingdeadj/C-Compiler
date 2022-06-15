#pragma once
#include "tools.h"
#include "tokentype.h"

//���ʽ��������Ϣ,����������
struct SemanticTypeInfo
{
    TokenType type = TYPE;
    string type_name;  // ��typeΪSTRUCT��TYPEʱ,type_name ��¼�����������,����id,int void float,char��
    bool is_const = false; //�Ƿ�Ϊ������
    bool is_array = false;  // �Ƿ�Ϊ��������


    string get_utf()const;

    bool is_struct()const {
        return type==STRUCT;
    }

    bool is_basic_type()const {
        return type==TYPE;
    }

    string str()const;


    //�ж��������������Ƿ�һ��,�����жϺ���������һ����
    bool is_same_return_type(const SemanticTypeInfo* infoB)const {
        auto infoA = this;
        assert(this->type_name != "" && infoB->type_name != "");
        return infoA->type == infoB->type
            && infoA->type_name == infoB->type_name
            && infoA->is_array == infoB->is_array
            && infoA->is_const == infoB->is_const
            ;
    }

    //�ж����������������Ƿ���ȫһ��
    bool is_same_para_type(const SemanticTypeInfo* infoB)const {
        auto infoA = this;
        assert(this->type_name != "" && infoB->type_name != "");
        return infoA->type == infoB->type
            && infoA->type_name == infoB->type_name
            && infoA->is_array == infoB->is_array
            && infoA->is_const == infoB->is_const
            ;
    }

    //���ڳ�ʼ���������ж�
    bool is_compatible_init(const SemanticTypeInfo* var_info) {
        assert(var_info->type == TYPE && var_info->type_name != "void" && var_info->type_name != "");
        return this->type == TYPE && !this->is_array
            && (
                this->type_name == var_info->type_name
                || this->type_name == "char"
                || (var_info->type_name == "float" && this->type_name == "int")
                )
            ;
    }

    // �жϸ�ֵ���Ҳ�������Ƿ����߼���
    bool is_compatible_with_left(const SemanticTypeInfo* other) {
        assert(this->type_name != "" && other->type_name != "");
        return
            this->type_name == other->type_name
            || other->type_name == "float"
            || (other->type_name == "int" && this->is_sub_type_of_int());
    }

    // �ж�ʵ�ε������Ƿ���βμ���
    bool is_compatible_with_para(const SemanticTypeInfo* other) {
        assert(this->type_name != "" && other->type_name != "");

        if (this->is_array != other->is_array || this->type != other->type
            || (this->is_array && this->type_name != other->type_name)
            )
            return false;

        if (other->type_name == "float")
            return this->is_sub_type_of_float();
        else if (other->type_name == "int")
            return this->is_sub_type_of_int();
        else
            return this->type_name == other->type_name;
    }

    bool is_sub_type_of_int()const {
        return (type_name == "int" || type_name == "char");
    }

    bool is_sub_type_of_float()const {
        return (type_name == "int" || type_name == "char" || type_name == "float");
    }

    bool compatible_with_int()const {
        assert(this->type_name != "");
        return (!is_array) && type == TYPE && is_sub_type_of_int();
    }

    bool compatible_with_float()const {
        assert(this->type_name != "");
        return (!is_array) && type == TYPE && is_sub_type_of_float()
            ;
    }

};

