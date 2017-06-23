#include<gecode/int.hh>
#include<gecode/search.hh>

#include "ourParser.h"


using namespace Gecode;

class Solver :public Space {

    VAL::domain* domain;
    VAL::problem* problem;

    public:

    void solveProblem() {




    }

    void setDomain(VAL::domain* dom) {
        domain = dom;
    }
    void setProblem(VAL::problem* prob) {
        problem = prob;
    }
};

int main(int argc, char** argv)
{
    OurParser parser;
    Solver *solver;

    solver->setDomain(parser.parseDomain(argv[1]));
    solver->setProblem(parser.parseProblem(argv[2]));

    solver->solveProblem();

    return 0;
}