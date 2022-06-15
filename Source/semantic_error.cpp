#include "charstream.h"
#include "semantic_error.h"

void show_error(const string& error_type, const Token* t, const string& des) {
    cerr << error_type
         <<" error in file "
        << (t->pos.stream->filename)
        << " line "
        << t->pos.line_no
        ;

    cerr << "\n";

    cerr << '\t' << des << "\n";

    success = false;
    throw false;
}

void show_error(const string& error_type, const Token* t, const ostringstream& oss) {
    show_error(error_type, t, oss.str());
}

void show_cg_error(const Token* t, const string& des) {
    show_error("Code generation",t,des);
}
void show_cg_error(const Token* t, const ostringstream& oss) {
    show_cg_error(t,oss.str());
}


void show_semantic_error(const Token* t, const string& des) {
    cerr << "Type checking error in file "
        << (t->pos.stream->filename)
        << " line "
        << t->pos.line_no
        ;

    cerr << "\n";

    cerr << '\t' << des << "\n";

    success = false;
    throw false;
}

void show_semantic_error(const Token* t, const ostringstream& oss) {
    show_semantic_error(t, oss.str());
}

