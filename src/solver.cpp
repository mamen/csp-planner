#include<gecode/int.hh>
#include<gecode/search.hh>

#include "ourParser.h"

#include <typeinfo>


using namespace Gecode;

class Solver :public Space {

    VAL::domain* domain;
    VAL::problem* problem;

    public:

    void solveProblem() {

        //1. get constants from domain
        //2. get possible actions (incl. parameters, preconditions and effects) from domain
        //3. get objects from problem
        //4. get initial state from problem
        //5. get goal state from problem
        //6. solve the problem
        //7. ...
        //8. profit?

        //1. get predicates from domain

        std::map<int,string> predicates; // id -> name

        int i = 0;

        while(domain->predicates->size() > 0) {
            predicates[i] = domain->predicates->front()->getPred()->getName();

            //cout <<  domain->predicates->front()->getArgs() << "\n";

            domain->predicates->pop_front();

            cout << predicates[i] << "\n";

            i++;
        }



        //cout << constants[0] << "\n";
        //cout << constants[1] << "\n";
        //cout << constants[2] << "\n";
        //cout << constants[3] << "\n";


        //cout << typeid(*domain->predicates).name();


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