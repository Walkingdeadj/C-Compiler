#pragma once
#include "tools.h"
#include "tokentype.h"

//表达式的类型信息,用于语义检查
struct SemanticTypeInfo
{
    TokenType type = TYPE;
    string type_name;  // 当type为STRUCT和TYPE时,type_name 记录具体的类型名,比如id,int void float,char等
    bool is_const = false; //是否为不变量
    bool is_array = false;  // 是否为数组类型


    string get_utf()const;

    bool is_struct()const {
        return type==STRUCT;
    }

    bool is_basic_type()const {
        return type==TYPE;
    }

    string str()const;


    //判断两个返回类型是否一致,用于判断函数声明的一致性
    bool is_same_return_type(const SemanticTypeInfo* infoB)const {
        auto infoA = this;
        assert(this->type_name != "" && infoB->type_name != "");
        return infoA->type == infoB->type
            && infoA->type_name == infoB->type_name
            && infoA->is_array == infoB->is_array
            && infoA->is_const == infoB->is_const
            ;
    }

    //判断两个参数的类型是否完全一致
    bool is_same_para_type(const SemanticTypeInfo* infoB)const {
        auto infoA = this;
        assert(this->type_name != "" && infoB->type_name != "");
        return infoA->type == infoB->type
            && infoA->type_name == infoB->type_name
            && infoA->is_array == infoB->is_array
            && infoA->is_const == infoB->is_const
            ;
    }

    //用于初始化的类型判断
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

    // 判断赋值号右侧的类型是否和左边兼容
    bool is_compatible_with_left(const SemanticTypeInfo* other) {
        assert(this->type_name != "" && other->type_name != "");
        return
            this->type_name == other->type_name
            || other->type_name == "float"
            || (other->type_name == "int" && this->is_sub_type_of_int());
    }

    // 判断实参的类型是否和形参兼容
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

