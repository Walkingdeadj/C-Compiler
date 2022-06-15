#include <cmath>
#include "tools.h"
#include "lexer.h"
#include "parser.h"

int main(int argc,char* argv[]) {
    #if 1
    #if 0
    part6_test();
    #else
    init_tokentype_table();

    if (argc < 2) {
        show_help();
        exit(0);
    }

    string mode_arg = argv[1];
    set_mode(mode_arg);
    
    set_ofs(argc, argv);

    if (mode == 0) {
        show_version();
        return 0;
    }

    //获取输入文件名
    set_infile(argc, argv);

    Lexer lexer(fullinfilenpath);
    lexer.do_parse();

    if (mode <3)
        exit(0);

    try {
        Parser* parser = new Parser();
        parser->do_parse();
        if (mode == 3){
            cout << "File " << fullinfilenpath << " is syntactically correct." << endl;
            exit(0);
        }

        parser->check_semantic();
        if (mode==4){
            parser->print_semantic_info();
            exit(0);
        }

        parser->pre_gen_code();
        parser->gen_code();

    }
    catch (bool) {
            exit(-1);
    }
    #endif
    #endif
    return 0;
}
