//
// Created by arne on 21.06.17.
//
#include "ptree.h"
#include "FlexLexer.h"
#include <iostream>
#include <fstream>

namespace VAL {
    analysis an_analysis;
    extern yyFlexLexer* yfl;
};

extern int yyparse();


VAL::domain* domainParse(char** argv) {

    VAL::domain* domain;
    VAL::current_analysis = &VAL::an_analysis;

    std::ifstream domainFile;
    domainFile.open(argv[1]);

    if (domainFile.fail()) {
        std::cerr << "Could not open domain file" << std::endl;
    } else {
        VAL::yfl = new yyFlexLexer;
        VAL::yfl->switch_streams(&domainFile, &std::cout);
        yyparse();
        domain = VAL::current_analysis->the_domain;
        std::cout << "Domain parsed: " << domain->name << std::endl;
        delete VAL::yfl;
    }
    domainFile.close();

    return domain;
}

VAL::problem* problemParse(char** argv) {

    VAL::problem* problem;
    VAL::current_analysis = &VAL::an_analysis;

    std::ifstream problemFile;
    problemFile.open(argv[2]);

    if (problemFile.fail()) {
        std::cerr << "Could not open problem file" << std::endl;
    } else {
        VAL::yfl = new yyFlexLexer;
        VAL::yfl->switch_streams(&problemFile, &std::cout);
        yyparse();
        problem = VAL::current_analysis->the_problem;
        std::cout << "problem Parsed: " << problem->name << std::endl;
        delete VAL::yfl;
    }
    problemFile.close();

    return problem;
}

int main(int argc, char** argv)
{

    VAL::domain* domain = domainParse(argv);
    VAL::problem* problem = problemParse(argv);

    return 0;
}