#pragma once
#include "tools.h"
#include "token.h"


void show_semantic_error(const Token* t, const string& des);
void show_semantic_error(const Token* t, const ostringstream& oss);
void show_cg_error(const Token* t, const string& des);
void show_cg_error(const Token* t, const ostringstream& oss);
