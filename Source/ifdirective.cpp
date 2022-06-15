#include "ifdirective.h"

stack<IfDirective*> if_dire_stack;

ostream& operator<<(ostream& os, const IfDirective& dire) {
    dire.print(os);
    return os;
}