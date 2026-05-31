#ifndef NODE_h__
#define NODE_h__

#include <string>
#include "Types.cpp"
#include "../StackAllocator/StackAllocator.hpp"

class Variable;

template<typename V1, typename V2>
class Duo{
public:
    V1 first;
    V2 second;
};

class Node{
public:
    NumberType type = NumberType::fixed;

    Node();
    //Plain prints the AST to the terminal.
    virtual void print() = 0;
    //Checks to see if there are only primitives in the nodes.
    virtual bool isEnd() = 0;
    //Performs a few evaluations to do "step by step" executions.
    virtual Duo<bool, Node*> step(StackAllocator* stack) = 0;
    //Prints out the AST showing what portions are to be evaluated next.
    virtual void stepPrint() = 0;
    virtual Number evaluate() = 0;
    //virtual Node* derivative(Variable* input) = 0;
};

class Primitive : public Node{
public:
    Number number;

    Primitive();
    void print();
    bool isEnd();
    Duo<bool, Node*> step(StackAllocator* stack);
    void stepPrint();
    Number evaluate();
};

class Imaginary : public Node{
public:
    Node* coefficient = nullptr;

    Imaginary();
    void print();
    bool isEnd();
    Duo<bool, Node*> step(StackAllocator* stack);
    void stepPrint();
    Number evaluate();
};

class Add : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Add();
    void print();
    bool isEnd();
    Duo<bool, Node*> step(StackAllocator* stack);
    void stepPrint();
    Number evaluate();
};

class Subtract : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Subtract();
    void print();
    bool isEnd();
    Duo<bool, Node*> step(StackAllocator* stack);
    void stepPrint();
    Number evaluate();
};

class Multiply : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Multiply();
    void print();
    bool isEnd();
    Duo<bool, Node*> step(StackAllocator* stack);
    void stepPrint();
    Number evaluate();
};

class Divide : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Divide();
    void print();
    bool isEnd();
    Duo<bool, Node*> step(StackAllocator* stack);
    void stepPrint();
    Number evaluate();
};

class Exponent : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Exponent();
    void print();
    bool isEnd();
    Duo<bool, Node*> step(StackAllocator* stack);
    void stepPrint();
    Number evaluate();
};

class Parentheses : public Node{
public:
    Node* subexpression = nullptr;

    Parentheses();
    void print();
    bool isEnd();
    Duo<bool, Node*> step(StackAllocator* stack);
    void stepPrint();
    Number evaluate();
};

class Negative : public Node{
public:
    Node* subexpression = nullptr;

    Negative();
    void print();
    bool isEnd();
    Duo<bool, Node*> step(StackAllocator* stack);
    void stepPrint();
    Number evaluate();
};

class Factorial : public Node{
public:
    Node* left = nullptr;

    Factorial();
    void print();
    bool isEnd();
    Duo<bool, Node*> step(StackAllocator* stack);
    void stepPrint();
    Number evaluate();
};

class AbsoluteValue : public Node{
public:
    Node* subexpression = nullptr;

    AbsoluteValue();
    void print();
    bool isEnd();
    Duo<bool, Node*> step(StackAllocator* stack);
    void stepPrint();
    Number evaluate();
};

class Variable : public Node{
public:
    std::string name = "";
    Number value;
    Variable* next = nullptr;
    Variable* previous = nullptr;

    Variable();
    void print();
    bool isEnd();
    Duo<bool, Node*> step(StackAllocator* stack);
    void stepPrint();
    Number evaluate();
};

#endif