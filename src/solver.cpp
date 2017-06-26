#include<gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>


#include "ourParser.h"


class Solver: public Gecode::IntMinimizeSpace
{
public:

    std::map<std::string, int> predicatesMap; // stores predicates as key -> values inside a map
    std::map<std::string, int> actionsMap; // stores actions as key -> values inside a map

    std::vector<Gecode::BoolVarArray> predicatesHistory_;
    std::vector<Gecode::BoolVarArgs> actionHistory_;

    /**
     * reads all neccessary data from the domain and stores it
     * in appropriate data-structures for gecode
     * @param domain the domain, from which the data will be read
     */
    void readDataFromDomain(int horizon, VAL::domain *domain) {
        int i = 0; // default loop-variable

        // get all predicates from the domain and save them into our predicatesMap
        for(list<VAL::pred_decl*>::iterator predicate = domain->predicates->begin(); predicate != domain->predicates->end(); predicate++)
        {
            predicatesMap.insert(std::pair<std::string, const int>((*predicate)->getPred()->getName(), i));
            i++;
            std::cout << i << ": " << (*predicate)->getPred()->getName() << "\n";
        }

        i = 0;

        // get all actions from the domain and save them into our actionsMap
        for(list<VAL::operator_*>::iterator action = domain->ops->begin(); action != domain->ops->end(); action++)
        {
            actionsMap.insert(std::pair<std::string, const int>((*action)->name->getName(), i));
            i++;
            std::cout << i << ": " << (*action)->name->getName() << "\n";
        }

        //construct a variable for each feature at each time step from 0 to horizon
        for( int i = 0; i < horizon; i++ ) {
            //add an array with all features for each step in the timeline
            predicatesHistory_.push_back(Gecode::BoolVarArray(*this, domain->predicates->size(), 0, 1));
        }

        //construct a boolean variable for each action at each time step from 0 to horizon - 1
        for( int i = 0; i < horizon - 1; i++ ) {
            actionHistory_.push_back(Gecode::BoolVarArray(*this, (int)domain->ops->size(), 0, 1));
        }
    }


    Gecode::BoolVar parsePreconditions(const VAL::goal* goal, int k, bool* success) {
        const VAL::simple_goal *simple = dynamic_cast<const VAL::simple_goal *>(goal);
        const VAL::conj_goal *conj = dynamic_cast<const VAL::conj_goal *>(goal);
        const VAL::neg_goal *neg = dynamic_cast<const VAL::neg_goal *>(goal);
        const VAL::disj_goal *disj = dynamic_cast<const VAL::disj_goal *>(goal);

        if (simple) {
            int id = predicatesMap.find(simple->getProp()->head->getName())->second;
            cout << "simple " << std::endl;
            return predicatesHistory_.at(k)[id];
        } else if (conj) {

            const VAL::goal_list *goals;
            goals = conj->getGoals();
            Gecode::BoolVar *tmp;

            int size = conj->getGoals()->size();

            if (size < 2) {
                *success = false;
                return Gecode::BoolVar();
            }

            tmp = new Gecode::BoolVar[size];
            int i = 0;
            for (auto precon = goals->begin(); precon != goals->end(); precon++) {
                tmp[i] = parsePreconditions(*precon, k, success);
                i++;
            }
            Gecode::BoolVar theOne = Gecode::expr(*this, (tmp[0]) && (tmp[1]));
            for(int j=2;j<i;j++){
                theOne = Gecode::expr(*this, theOne && (tmp[j]));
            }
            cout << "conjunction " << std::endl;
            return theOne;

        } else if (neg) {
            Gecode::BoolVar test = Gecode::expr(*this, not(parsePreconditions(neg->getGoal(), k, success)));
            cout << "negative" << std::endl;
            return test;
        } else if (disj) {
            cout << "disj" << std::endl;
            const VAL::goal_list *goals;
            goals = conj->getGoals();
            Gecode::BoolVar *tmp;
            int size = conj->getGoals()->size();

            if (size < 2) {
                *success = false;
                return Gecode::BoolVar();
            }

            tmp = new Gecode::BoolVar[size];
            int i = 0;
            for (auto precon = goals->begin(); precon != goals->end(); precon++) {
                tmp[i] = parsePreconditions(*precon, k, success);
                i++;
            }
            Gecode::BoolVar theOne = Gecode::expr(*this, (tmp[0]) || (tmp[1]));
            for(int j=2;j<i;j++){
                theOne = Gecode::expr(*this, theOne || (tmp[j]));
            }
            cout << "disjunction " << std::endl;
            return theOne;
        }
    }


    // our own solver
    Solver(VAL::domain *domain, VAL::problem *problem)
    {
        // --- Sources ---
        // [1] http://artint.info/html/ArtInt_208.html

        int horizon = 10; // To construct a CSP from a planning problem, first choose a fixed horizon, which is the number of time steps over which to plan. [1]

        readDataFromDomain(horizon, domain);

        // for each time point, connect preconditions with actions in same time window
        for( int t = 0; t < horizon - 1; ++t ) {
            for(auto a = domain->ops->begin();
                a != domain->ops->end();
                ++a)
            {
                //std::cout << "Preconditions for: " << (*a)->name->getName() << std::endl;
                (*a)->precondition->display(0);

                // parse preconditions
                bool success = true;
                Gecode::BoolVar x = parsePreconditions((*a)->precondition, t, &success);
                if (success) {
                    Gecode::rel(*this, x);
                } else {

                    cout << "precondition empty" << std::endl;
                }
                // use result to define a gecode relation

                std::cout << std::endl;
            }
        }


        //effect constraints between state variables at time t, action variables at time t and state variables at time t + 1
        int x = 0;
        for( int t = 0; t < horizon - 1; ++t ) {

            Gecode::rel(*this, sum(actionHistory_.at(t)) <= 1);

            for(auto a = domain->ops->begin();
                a != domain->ops->end();
                ++a)
            {
                int aId = actionsMap.find((*a)->name->getName())->second;
                for (auto i = (*a)->effects->add_effects.begin(); i != (*a)->effects->add_effects.end(); i++) {
                    int id = predicatesMap.find((*i)->prop->head->getName())->second;
                    Gecode::rel(*this, predicatesHistory_.at(t + 1)[id] << actionHistory_.at(t)[aId]);
                }

                for (auto i = (*a)->effects->del_effects.begin(); i != (*a)->effects->del_effects.end(); i++) {
                    int id = predicatesMap.find((*i)->prop->head->getName())->second;
                    Gecode::rel(*this, !predicatesHistory_.at(t + 1)[id] << actionHistory_.at(t)[aId]);
                }

                std::cout << std::endl;
            }
        }

        //initial state constraints
        for(auto is = problem->initial_state->add_effects.begin(); is != problem->initial_state->add_effects.end(); ++is)
        {
            string effectName = (*is)->prop->head->getName();
            std::cout << "EFFECT INITIAL:"  << (*is)->prop->head->getName() << std::endl;
            //get initial effect id
            if ( predicatesMap.find(effectName) != predicatesMap.end() ) {

                int effectId = predicatesMap.find(effectName)->second;
                std::cout << "found at " << effectId;
                //constraint feature at time 0
                Gecode::rel(*this, predicatesHistory_.at(0)[effectId] == 1);
            }
        }


        //goal state constraints
        bool success2 = true;
        Gecode::rel(*this, parsePreconditions(problem->the_goal, horizon - 1, &success2));

        rg_ = Gecode::Rnd(time(NULL));

        Gecode::branch(*this, actionHistory_.front(), Gecode::INT_VAR_RND(rg_),  Gecode::INT_VAL_RND(rg_));

        options_.threads = 4;

        search_ = new Gecode::DFS<Solver>(this, options_) ;

        solution_ = NULL;





        rg_ = Gecode::Rnd(time(NULL));

        steps_ = Gecode::IntVarArray(*this, 4 , 0, 10);

        Gecode::rel(*this, steps_[0]==steps_[1]);
        Gecode::rel(*this, (steps_[1]!=steps_[2]) >> (steps_[3]==3));

        Gecode::branch(*this, steps_, Gecode::INT_VAR_RND(rg_),  Gecode::INT_VAL_RND(rg_));

        options_.threads = 4;

        search_ = new Gecode::DFS<Solver>(this, options_) ;

        solution_ = NULL;

    }

    // default constructor, taken from the csp_test.cpp
    Solver(bool share, Solver & s) : Gecode::IntMinimizeSpace(share,s)
    {
        steps_.update(*this, share, s.steps_);
    }

    // the copy function, taken from the csp_test.cpp
    Gecode::IntMinimizeSpace * copy(bool share)
    {
        return new Solver(share,*this);
    }

    // not used function, but has to stay in or else it wont compile
    // ignore the "No matching constructor" error, it still compiles fine
    Gecode::IntVar cost(void) const
    {
        return Gecode::IntVar(0);
    }

    // the solve function, taken from the csp_test.cpp
    bool solve ()
    {
        if (solution_)
        {
            delete solution_;
        }
        solution_ = search_->next();

        if (!solution_)
        {
            return false;
        }
        return true;
    }

    // taken from the csp_test.cpp
    Gecode::IntVarArray steps_;         // an IntVarArray with all taken steps
    Gecode::DFS<Solver> * search_;
    Solver * solution_;
    Gecode::Rnd rg_;                    // a randome number
    Gecode::Search::Options options_;



};

int main(int argc, char ** argv)
{

    OurParser parser;

    VAL::domain *domain = parser.parseDomain(argv[1]);
    VAL::problem *problem = parser.parseProblem(argv[2]);
    Solver solver(domain, problem);

    // the final number of solution
    int solutionCnt = 0;

    while(solver.solve())
    {
        solutionCnt ++;
        //std::cout << "solution: " << solver.solution_->steps_[0].val() << " " << solver.solution_->steps_[1].val() << " " << solver.solution_->steps_[2].val() << " " << solver.solution_->steps_[3].val() << " " << std::endl;
    }
    std::cout << solutionCnt << " solutions found" <<  std::endl;



    return 0;

}
