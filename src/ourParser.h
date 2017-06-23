#include "ptree.h"
#include "FlexLexer.h"
#include <iostream>
#include <fstream>

class OurParser {
    public: VAL::domain* parseDomain(char *arg);
    public: VAL::problem* parseProblem(char *arg);
};

