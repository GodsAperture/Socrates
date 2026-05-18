#ifndef NODE_h__
#include <string>
#include "Types.cpp"

class Variable;

class Node{
public:
    NumberType type = NumberType::NULLTYPE;

    Node();
    void virtual print() = 0;
    Number virtual evaluate() = 0;
    //Node* derivative(Variable* input) = 0;
};

class Primitive : public Node{
public:
    Number number;

    Primitive();
    void print();
    Number evaluate();
};

class Add : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Add();
    void print();
    Number evaluate();
};

class Subtract : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Subtract();
    void print();
    Number evaluate();
};

class QuietMultiply : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    QuietMultiply();
    void print();
    Number evaluate();
};

class Multiply : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Multiply();
    void print();
    Number evaluate();
};

class Divide : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Divide();
    void print();
    Number evaluate();
};

class Exponent : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Exponent();
    void print();
    Number evaluate();
};

class Parentheses : public Node{
public:
    Node* subexpression = nullptr;

    Parentheses();
    void print();
    Number evaluate();
};

class Negative : public Node{
public:
    Node* subexpression = nullptr;

    Negative();
    void print();
    Number evaluate();
};

class Variable : public Node{
public:
    std::string name = "";
    Number value;
    Variable* next = nullptr;

    Variable();
    void print();
    Number evaluate();
};

#endif