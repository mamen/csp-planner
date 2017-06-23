//
// Created by arne on 22.06.17.
//
#include "ourParser.h"

namespace VAL {
    analysis an_analysis;
    extern yyFlexLexer* yfl;
};

extern int yyparse();

VAL::domain* OurParser::parseDomain(char* arg) {

    VAL::domain* domain;
    VAL::current_analysis = &VAL::an_analysis;

    std::ifstream domainFile;
    domainFile.open(arg);

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

VAL::problem* OurParser::parseProblem(char* arg) {

    VAL::problem* problem;
    VAL::current_analysis = &VAL::an_analysis;

    std::ifstream problemFile;
    problemFile.open(arg);

    if (problemFile.fail()) {
        std::cerr << "Could not open problem file" << std::endl;
    } else {
        VAL::yfl = new yyFlexLexer;
        VAL::yfl->switch_streams(&problemFile, &std::cout);
        yyparse();
        problem = VAL::current_analysis->the_problem;
        std::cout << "problem parsed: " << problem->name << std::endl;
        delete VAL::yfl;
    }
    problemFile.close();

    return problem;
}

