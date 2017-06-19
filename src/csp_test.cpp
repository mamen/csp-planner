#include<gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>


class csp_test: public Gecode::IntMinimizeSpace
{
    public:

    csp_test()
    {

        rg_ = Gecode::Rnd(time(NULL));

        numbers_ = Gecode::IntVarArray(*this, 4 , 0, 10);

        Gecode::rel(*this, numbers_[0]==numbers_[1]);
        Gecode::rel(*this, (numbers_[1]!=numbers_[2]) >> (numbers_[3]==3));

        Gecode::branch(*this, numbers_, Gecode::INT_VAR_RND(rg_),  Gecode::INT_VAL_RND(rg_));

        options_.threads = 4;

        search_ = new Gecode::DFS<csp_test>(this, options_) ;

        solution_ = NULL;

    }

    csp_test(bool share, csp_test & s) : Gecode::IntMinimizeSpace(share,s)
    {
        numbers_.update(*this, share, s.numbers_);
    }

    Gecode::IntMinimizeSpace * copy(bool share)
    {
        return new csp_test(share,*this);
    }

    Gecode::IntVar cost(void) const
    {
        return Gecode::IntVar(0);
    }

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

    Gecode::IntVarArray numbers_;
    Gecode::DFS<csp_test> * search_;
    csp_test * solution_;
    Gecode::Rnd rg_;
    Gecode::Search::Options options_;

};

int main(int argc, char ** argv)
{

    csp_test ct;

    int num_sol = 0;

    while(ct.solve())
    {
        num_sol ++;
        std::cout << "solution: " << ct.solution_->numbers_[0].val() << " " << ct.solution_->numbers_[1].val() << " " << ct.solution_->numbers_[2].val() << " " << ct.solution_->numbers_[3].val() << " " << std::endl;
    }
    std::cout << num_sol << " solutions found" <<  std::endl;



    return 0;

}
