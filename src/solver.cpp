//
// Created by arne on 21.06.17.
//
#include "ourParser.h"

int main(int argc, char** argv)
{
    OurParser parser;
    VAL::domain* domain = parser.parseDomain(argv[1]);
    VAL::problem* problem = parser.parseProblem(argv[2]);

    return 0;
}