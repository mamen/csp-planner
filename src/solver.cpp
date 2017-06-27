#include<gecode/int.hh>
#include <gecode/minimodel.hh>

#include "ourParser.h"

class Solver: public Gecode::IntMinimizeSpace {
    public:

        bool showOutput = false;

        /**
         * reads all necessary data from the domain and stores it
         * in appropriate data-structures to use with gecode
         * @param domain the domain, from which the data will be read
         */
        void readDataFromDomain(VAL::domain *domain) {
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
        }

        /**
         * parses a precondition
         * @param goal
         * @param horizon
         * @param success
         * @return
         */
        Gecode::BoolVar parsePrecondition(const VAL::goal *goal, int horizon, bool *error) {
            // check if goal is:
            //      - simple
            //      - a conjunction
            //      - a negation
            //      - a disjunction

            const VAL::simple_goal *simpleGoal = dynamic_cast<const VAL::simple_goal *>(goal);
            const VAL::conj_goal *conjunctionGoal = dynamic_cast<const VAL::conj_goal *>(goal);
            const VAL::neg_goal *negationGoal = dynamic_cast<const VAL::neg_goal *>(goal);
            const VAL::disj_goal *disjunctionGoal = dynamic_cast<const VAL::disj_goal *>(goal);

            if(showOutput)
                cout << "goal: \n";

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
                // this creates a new Gecode expression with the boolean value of the negated goal
                return Gecode::expr(*this, not(parsePrecondition(negationGoal->getGoal(), horizon, error)));
            }

            // if the goal is a conjunction
            if(conjunctionGoal) {
                if(showOutput)
                    cout << *conjunctionGoal << "\n --- \n";

                // get all the goals
                const VAL::goal_list *goals = conjunctionGoal->getGoals();

                // number of goals
                int goalCount = conjunctionGoal->getGoals()->size();

                if(goalCount < 2) {
                    *error = true;
                    return Gecode::BoolVar();
                }

                // temporary BoolVar-array for nested subgoals
                Gecode::BoolVar *temp = new Gecode::BoolVar[goalCount];

                int i = 0;

                // for each element of the conjunction, parse it again to see, if there are nested goals in it
                for (list<VAL::goal*>::const_iterator precondition = goals->begin(); precondition != goals->end(); precondition++) {
                    temp[i] = parsePrecondition(*precondition, horizon, error); // recursively parse the nested condition
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

                if(goalCount < 2) {
                    *error = true;
                    return Gecode::BoolVar();
                }

                Gecode::BoolVar *temp = new Gecode::BoolVar[goalCount];

                int i = 0;

                // for each element of the disjunction, parse it again to see, if there are nested goals in it
                for (list<VAL::goal*>::const_iterator precondition = goals->begin(); precondition != goals->end(); precondition++) {
                    temp[i] = parsePrecondition(*precondition, horizon, error);
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
            /*
             *
             * [1] get the predicates and actions from the domain
             * [2] create the history-vectors for the predicates and actions
             * [3] connect predicates <-> actions
             * [4] connect action <-> effect
             * [5] initial state constraints
             * [6] goal-state relations
             * [7] solve
             *
             * --- Existing Problems ---
             * Branching is not possible (No matching function, although it has the same
             * parameter-types as in the given example)
             *
             * The solver finds a correct solution, but somehow does not save the taken steps
             *
             */

            int horizon = 20; // To construct a CSP from a planning problem, first choose a fixed horizon, which is
                              // the number of time steps over which to plan (a fixed number of steps)
                              // Source: http://artint.info/html/ArtInt_208.html

            //IntVarArray(Space & home, int n, int min, int max)
            steps_ = Gecode::IntVarArray(*this, horizon , 0, 1);

            // read predicates and actions from the domain and create the historys
            readDataFromDomain(domain);

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

            // connect precondition to action
            for(int i = 0; i < horizon - 1; i++) {

                // iterate over all actions
                for(list<VAL::operator_*>::iterator action = domain->ops->begin(); action != domain->ops->end(); ++action) {

                    // parse the preconditions
                    bool error = false;
                    Gecode::BoolVar setPrecondition = parsePrecondition((*action)->precondition, i, &error);

                    // define the relationship, that the precondition has to be given
                    if(!error) {
                        Gecode::rel(*this, setPrecondition);
                    }
                }
            }

            // connect relation between actions and effects for each part of the horizon i
            for( int i = 0; i < horizon - 1; i++) {

                // can only make one action at a time
                Gecode::rel(*this, sum(actionHistory.at(i)) <= 1);

                // iterate over all actions
                for(list<VAL::operator_*>::iterator action = domain->ops->begin(); action != domain->ops->end(); ++action) {
                    // get the actionID
                    int actionID = actionsMap.find((*action)->name->getName())->second;

                    // iterate over all effects to be added of the current action
                    // e.g. grab adds the effect hasbanana in the monkey-example
                    for(list<VAL::simple_effect*>::iterator effect = (*action)->effects->add_effects.begin(); effect != (*action)->effects->add_effects.end(); ++effect) {
                        int id = predicatesMap.find((*effect)->prop->head->getName())->second;

                        // set relation for action -> effect
                        // implication <<
                        // http://www.hakank.org/constraint_programming_blog/2010/07/about_100_new_gecode_models_1.html
                        Gecode::rel(*this, predicatesHistory.at(i + 1)[id] << actionHistory.at(i)[actionID]);
                    }

                    // iteratre over all effects to be deleted of the current action
                    // e.g. climb removes the effect on-floor in the monkey-example
                    for(list<VAL::simple_effect*>::iterator effect = (*action)->effects->del_effects.begin(); effect != (*action)->effects->del_effects.end(); ++effect) {
                        int id = predicatesMap.find((*effect)->prop->head->getName())->second;

                        // set relation for action -> effect
                        // implication <<
                        // http://www.hakank.org/constraint_programming_blog/2010/07/about_100_new_gecode_models_1.html
                        Gecode::rel(*this, !predicatesHistory.at(i + 1)[id] << actionHistory.at(i)[actionID]);
                    }

                }
            }


            // create the constraints for the initial state
            for(list<VAL::simple_effect*>::iterator initialState = problem->initial_state->add_effects.begin(); initialState != problem->initial_state->add_effects.end(); ++initialState) {

                // get the initial state name
                std::string effectName = (*initialState)->prop->head->getName();

                // get the effect id
                int effectId = predicatesMap.find(effectName)->second;

                // set relation at the beginning (0)
                // so that these effects are given
                Gecode::rel(*this, predicatesHistory.at(0)[effectId] == 1);
            }

            bool error = false;

            // set relation for the goal state
            Gecode::rel(*this, parsePrecondition(problem->the_goal, horizon - 1, &error));

            // taken from csp_test.cpp
            rg_ = Gecode::Rnd(time(NULL));

            // TODO Branching somehow does not work, idk why
            //Gecode::branch(*this, steps_, Gecode::INT_VAL_RND(rg_),  Gecode::INT_VAL_RND(rg_));

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

        std::vector<Gecode::BoolVarArray> predicatesHistory;
        std::vector<Gecode::BoolVarArgs> actionHistory;

        std::map<std::string, int> predicatesMap;   // stores predicates as value -> key inside a map
        std::map<std::string, int> actionsMap;      // stores actions as value -> key inside a map

        // taken from the csp_test.cpp
        Gecode::IntVarArray steps_;         // an IntVarArray with all taken steps
        Gecode::DFS<Solver> * search_;
        Solver * solution_;
        Gecode::Rnd rg_;                    // a random number
        Gecode::Search::Options options_;

};


int main(int argc, char ** argv) {

    OurParser parser;

    VAL::domain *domain = parser.parseDomain(argv[1]);
    VAL::problem *problem = parser.parseProblem(argv[2]);
    Solver solver(domain, problem);

    int solutionCnt = 0;

    while(solver.solve())
    {
        solutionCnt ++;

//        cout << solver.solution_->steps_[0].val() << "\n";
//
//        if(solver.solution_->steps_.size() > 0) {
//            for(std::vector<Gecode::BoolVarArray>::size_type i = 0; i != solver.actionHistory.size(); i++) {
//                std::cout << "step #"<< i << ": " << solver.solution_->steps_[i].val() << "\n";
//            }
//        }

    }

    if(solutionCnt == 1) {
        std::cout << "The solver found " << solutionCnt << " solution for the given domain and problem" <<  std::endl;
    } else {
        std::cout << "The solver found " << solutionCnt << " solutions for the given domain and problem" <<  std::endl;
    }


    return 0;

}
