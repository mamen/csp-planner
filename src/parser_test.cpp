#include "ptree.h"
#include "FlexLexer.h"

#include <iostream>
#include <fstream>

extern int yyparse();
extern int yydebug;

namespace VAL {
	analysis an_analysis;
	extern yyFlexLexer* yfl;
};


int main(int argc, char ** argv)
{

    VAL::analysis * val_analysis;
    VAL::problem* problem;
    VAL::domain* domain;


    VAL::current_analysis = val_analysis = &VAL::an_analysis;
    std::ifstream domainFile;
    domainFile.open(argv[1]);
    yydebug = 0;
    
    VAL::yfl = new yyFlexLexer;

    if (domainFile.fail()) 
    {
        line_no = 0;
        std::cerr << "Failed to open domain file" << std::endl;
    } 
    else 
    {
	line_no = 1;
	VAL::yfl->switch_streams(&domainFile, &std::cout);
	yyparse();

	domain = VAL::current_analysis->the_domain;
    }

    std::cout << "domain: " << domain->name << std::endl;

    domain->write(std::cout);

    delete VAL::yfl;
    domainFile.close();

    std::ifstream problemFile;
    problemFile.open(argv[2]);
    yydebug = 0;
    
    VAL::yfl = new yyFlexLexer;

    if (problemFile.fail()) 
    {
        line_no = 0;
        std::cerr << "Failed to open problem file" << std::endl;
    } 
    else 
    {
	line_no = 1;
	VAL::yfl->switch_streams(&problemFile, &std::cout);
	yyparse();

	problem = VAL::current_analysis->the_problem;
    }

    std::cout << std::endl;


    std::cout << "problem: " << problem->name << std::endl;

    problem->write(std::cout);

    std::cout << std::endl;

    delete VAL::yfl;
    problemFile.close();

    return 0;
}
