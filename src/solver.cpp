//
// Created by arne on 21.06.17.
//
#include "ptree.h"
#include "FlexLexer.h"
#include <fstream>

extern int yyparse();

namespace VAL {
    extern yyFlexLexer* yfl;
};

int main(int argc, char ** argv)
{

    VAL::domain* domain;
    VAL::problem* problem;

    VAL::current_analysis = &VAL::an_analysis;

    std::ifstream domainFile;
    domainFile.open(argv[1]);

    std::ifstream problemFile;
    problemFile.open(argv[2]);

    if (domainFile.fail()) {
        std::cerr << "Could not open domain file" << std::endl;
    } else {
        VAL::yfl = new yyFlexLexer;
        VAL::yfl->switch_streams(&domainFile, &std::cout);
        yyparse();
        domain = VAL::current_analysis->the_domain;
        std::cout << "Domain parsed :" << domain->name << std::endl;
        delete VAL::yfl;
    }
    domainFile.close();

    if (problemFile.fail()) {
        std::cerr << "Could not open problem file" << std::endl;
    } else {
        VAL::yfl = new yyFlexLexer;
        VAL::yfl->switch_streams(&problemFile, &std::cout);
        yyparse();
        domain = VAL::current_analysis->the_problem;
        std::cout << "problem parsed :" << problem->name << std::endl;
        delete VAL::yfl;
    }
    problemFile.close();

    return 0;
}