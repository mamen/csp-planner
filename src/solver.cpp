#include<gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>


#include "ourParser.h"

class Solver: public Gecode::IntMinimizeSpace
{
public:

    bool showOutput = false;

    std::map<std::string, int> predicatesMap;   // stores predicates as key -> values inside a map
    std::map<std::string, int> actionsMap;      // stores actions as key -> values inside a map

    std::vector<Gecode::BoolVarArray> predicatesHistory;
    std::vector<Gecode::BoolVarArray> actionHistory;

    /**
     * reads all necessary data from the domain and stores it
     * in appropriate data-structures to use with gecode
     * @param domain the domain, from which the data will be read
     */
    void readDataFromDomain(int horizon, VAL::domain *domain) {
        int i = 0; // default loop-variable

        if(showOutput)
            cout << "found predicates: \n";
        // get all predicates from the domain and save them into our predicatesMap
        for(list<VAL::pred_decl*>::iterator predicate = domain->predicates->begin(); predicate != domain->predicates->end(); predicate++) {
            predicatesMap.insert(std::pair<std::string, const int>((*predicate)->getPred()->getName(), i));
            i++;
            if(showOutput)
                std::cout << i << ": " << (*predicate)->getPred()->getName() << "\n";
        }

        i = 0;

        if(showOutput)
            cout << "\nfound actions: \n";
        // get all actions from the domain and save them into our actionsMap
        for(list<VAL::operator_*>::iterator action = domain->ops->begin(); action != domain->ops->end(); action++)
        {
            actionsMap.insert(std::pair<std::string, const int>((*action)->name->getName(), i));
            i++;
            if(showOutput)
                std::cout << i << ": " << (*action)->name->getName() << "\n";
        }

        //construct |horizon| BoolVarArrays for our predicatesHistory
        for( int i = 0; i < horizon; i++ ) {
            // constructor:
            // Gecode::BoolVarArray(Space & home, int n, int min, int max)

            predicatesHistory.push_back(Gecode::BoolVarArray(*this, domain->predicates->size(), 0, 1));
        }

        //construct |horizon| BoolVarArrays for our actionHistory
        for( int i = 0; i < horizon - 1; i++ ) {
            // constructor:
            // Gecode::BoolVarArray(Space & home, int n, int min, int max)
            actionHistory.push_back(Gecode::BoolVarArray(*this, (int)domain->ops->size(), 0, 1));
        }
    }

    /**
     * parses a precondition
     * @param goal
     * @param horizon
     * @param success
     * @return
     */
    Gecode::BoolVar parsePreconditions(const VAL::goal* goal, int horizon) {
        // check if goal is:
        //      - simple
        //      - a conjunction
        //      - a negation
        //      - a disjunction

        const VAL::simple_goal *simpleGoal = dynamic_cast<const VAL::simple_goal *>(goal);
        const VAL::conj_goal *conjunctionGoal = dynamic_cast<const VAL::conj_goal *>(goal);
        const VAL::neg_goal *negationGoal = dynamic_cast<const VAL::neg_goal *>(goal);
        const VAL::disj_goal *disjunctionGoal = dynamic_cast<const VAL::disj_goal *>(goal);

        // if the goal is a simple one
        if(simpleGoal) {
            if(showOutput)
                cout << *simpleGoal << "\n --- \n";

            // get the index of the predicate in our predicateMap
            int id = predicatesMap.find(simpleGoal->getProp()->head->getName())->second;

            // return the history-entry for this predicate
            return predicatesHistory.at(horizon)[id];
        }

        // if the goal is a negation
        if(negationGoal) {
            if(showOutput)
                cout << *negationGoal << "\n --- \n";
            // return the expression for the negation of the precondition
            return Gecode::expr(*this, not(parsePreconditions(negationGoal->getGoal(), horizon)));
        }

        // if the goal is a conjunction
        if(conjunctionGoal) {
            if(showOutput)
                cout << *conjunctionGoal << "\n --- \n";

            // get all the goals
            const VAL::goal_list *goals = conjunctionGoal->getGoals();

            // number of goals
            int goalCount = conjunctionGoal->getGoals()->size();

            // temporary BoolVar-array for nested subgoals
            Gecode::BoolVar *temp = new Gecode::BoolVar[goalCount];

            int i = 0;

            // for each element of the conjunction, parse it again to see, if there are nested goals in it
            for (list<VAL::goal*>::const_iterator precondition = goals->begin(); precondition != goals->end(); precondition++) {
                temp[i] = parsePreconditions(*precondition, horizon);
                i++;
            }

            // create the expressions for the conjunction
            Gecode::BoolVar result = Gecode::expr(*this, (temp[0]) && (temp[1]));

            // if there are nested conjunctions (i > 0), create an expression for those aswell
            for(int j = 2; j < i ; j++){
                result = Gecode::expr(*this, result && (temp[j]));
            }

            return result;
        }

        // if the goal is a disjunction
        if(disjunctionGoal) {
            if(showOutput)
                cout << *disjunctionGoal << "\n --- \n";

            // get all goals of the disjunction
            const VAL::goal_list *goals = disjunctionGoal->getGoals();

            // number of subgoals
            int goalCount = disjunctionGoal->getGoals()->size();

            Gecode::BoolVar *temp = new Gecode::BoolVar[goalCount];


            int i = 0;

            // for each element of the disjunction, parse it again to see, if there are nested goals in it
            for (list<VAL::goal*>::const_iterator precondition = goals->begin(); precondition != goals->end(); precondition++) {
                temp[i] = parsePreconditions(*precondition, horizon);
                i++;
            }

            // create the expressions for the conjunction
            Gecode::BoolVar result = Gecode::expr(*this, (temp[0]) || (temp[1]));

            // if there are nested conjunctions (i > 0), create an expression for those aswell
            for(int j = 2; j < i ; j++){
                result = Gecode::expr(*this, result || (temp[j]));
            }

            return result;
        }
    }

    /**
     * constructor for our solver
     * @param domain the domain
     * @param problem the problem
     */
    Solver(VAL::domain *domain, VAL::problem *problem)
    {
        // --- Sources ---
        // [1] http://artint.info/html/ArtInt_208.html

        int horizon = 10; // To construct a CSP from a planning problem, first choose a fixed horizon, which is the number of time steps over which to plan. [1]

        // read predicates and actions from the domain and create the historys
        readDataFromDomain(horizon, domain);

        // connect precondition to action
        for(int t = 0; t < horizon - 1; t++) {

            // iterate over all actions
            for(list<VAL::operator_*>::iterator action = domain->ops->begin(); action != domain->ops->end(); ++action) {

                // boolean to check, if a precondition was found
                bool success = true;

                // parse the preconditions
                //cout << "action: " << *(*action)->name;
                Gecode::BoolVar setPrecondition = parsePreconditions((*action)->precondition, t);

                // define the relationship, that the precondition has to be given
                Gecode::rel(*this, setPrecondition);
            }
        }













        //effect constraints between state variables at time t, action variables at time t and state variables at time t + 1
        int x = 0;
        for( int t = 0; t < horizon - 1; ++t ) {

            Gecode::rel(*this, sum(actionHistory.at(t)) <= 1);

            for(auto a = domain->ops->begin();
                a != domain->ops->end();
                ++a)
            {
                int aId = actionsMap.find((*a)->name->getName())->second;
                for (auto i = (*a)->effects->add_effects.begin(); i != (*a)->effects->add_effects.end(); i++) {
                    int id = predicatesMap.find((*i)->prop->head->getName())->second;
                    Gecode::rel(*this, predicatesHistory.at(t + 1)[id] << actionHistory.at(t)[aId]);
                }

                for (auto i = (*a)->effects->del_effects.begin(); i != (*a)->effects->del_effects.end(); i++) {
                    int id = predicatesMap.find((*i)->prop->head->getName())->second;
                    Gecode::rel(*this, !predicatesHistory.at(t + 1)[id] << actionHistory.at(t)[aId]);
                }

                //std::cout << std::endl;
            }
        }

        //initial state constraints
        for(auto is = problem->initial_state->add_effects.begin(); is != problem->initial_state->add_effects.end(); ++is)
        {
            string effectName = (*is)->prop->head->getName();
           // std::cout << "EFFECT INITIAL:"  << (*is)->prop->head->getName() << std::endl;
            //get initial effect id
            if ( predicatesMap.find(effectName) != predicatesMap.end() ) {

                int effectId = predicatesMap.find(effectName)->second;
                std::cout << "found at " << effectId << "\n";
                //constraint feature at time 0
                Gecode::rel(*this, predicatesHistory.at(0)[effectId] == 1);
            }
        }


        //goal state constraints
        Gecode::rel(*this, parsePreconditions(problem->the_goal, horizon - 1));


        // taken from csp_test.cpp

        rg_ = Gecode::Rnd(time(NULL));

        //Gecode::branch(*this, actionHistory_.front(), Gecode::INT_VAR_RND(rg_),  Gecode::INT_VAL_RND(rg_));

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
    Gecode::Rnd rg_;                    // a random number
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
    }
    std::cout << solutionCnt << " solutions found" <<  std::endl;



    return 0;

}
