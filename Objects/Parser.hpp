#ifndef PARSER_h__
#define PARSER_h__

#include <cstdint>
#include <vector>
#include "../Lexer/Token.hpp"
#include "Node.hpp"
#include "../StackAllocator/StackAllocator.hpp"

class Parser{
public:
    int64_t current = 0;
    std::vector<Token> given;
    StackAllocator* stack = nullptr;
    Node* AST = nullptr;

    Parser();
    inline bool check(std::string input);

    void parse();
    void print();
    void clear();
    Node* PAdd();
    Node* PSubtract();
    Node* PImaginary();
    Node* PMultiply();
    Node* PDivide();
    Node* PNegative();
    Node* PExponent();
    Node* PFactorial();
    Node* PPrimitive();
    Node* PParentheses();
    Node* PAbsoluteValue();
};
#endif