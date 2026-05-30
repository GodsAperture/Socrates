#ifndef ARITHMETIC_h__
#define ARITHMETIC_h__
#include "Problem.hpp"

class Arithmetic : public Problem{
    Arithmetic();
    Node* generate();
    void prompt();
    void respond();
};
#endif