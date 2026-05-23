#include <iostream>
#include "Parser.hpp"

Parser::Parser(std::vector<Token>& input){
    given = input;
    stack = new StackAllocator(1024);
}



void Parser::parse(){
    AST = PAdd();
}

void Parser::print(){
    AST->print();
}



inline bool Parser::check(std::string input){
    if(input == given[current].name){
        current++;
        return true;
    }

    return false;
}

Node* Parser::PAdd(){
    Node* left = nullptr;
    Node* right = nullptr;
    Add* result = nullptr;

    left = PSubtract();
    if(left == nullptr){
        return nullptr;
    }

    if(!check("+")){
        return left;
        std::cout << "Error, bad syntax.\nFailed to parse after:\n\t";
        left->print();
    }

    right = PAdd();
    if(right == nullptr){
        return nullptr;
        std::cout << "Error, bad syntax.\n";
    }

    result = stack->allocate<Add>();
    result->left = left;
    result->right = right;

    return result;

}

Node* Parser::PSubtract(){
    Node* left = nullptr;
    Node* right = nullptr;
    Subtract* result = nullptr;

    left = PMultiply();
    if(left == nullptr){
        return nullptr;
    }

    if(!check("-")){
        return left;
        std::cout << "Error, bad syntax.\nFailed to parse after:\n\t";
        left->print();
    }

    right = PSubtract();
    if(right == nullptr){
        return nullptr;
        std::cout << "Error, bad syntax.\n";
    }

    result = stack->allocate<Subtract>();
    result->left = left;
    result->right = right;

    return result;

}

Node* Parser::PMultiply(){
    Node* left = nullptr;
    Node* right = nullptr;
    Multiply* result = nullptr;

    left = PDivide();
    if(left == nullptr){
        return nullptr;
    }

    if(!check("*")){
        right = PMultiply();
        if(right != nullptr){
            QuietMultiply* newResult = stack->allocate<QuietMultiply>();
            newResult->left = left;
            newResult->right = right;

            return newResult;
        } else {
            return left;
        }
    }

    right = PMultiply();
    if(right == nullptr){
        return nullptr;
        std::cout << "Error, bad syntax.\n";
    }

    result = stack->allocate<Multiply>();
    result->left = left;
    result->right = right;

    return result;

}

Node* Parser::PDivide(){
    Node* left = nullptr;
    Node* right = nullptr;
    Divide* result = nullptr;

    left = PExponent();
    if(left == nullptr){
        return nullptr;
    } 

    if(!check("/")){
        return left;
    }

    right = PDivide();
    if(right == nullptr){
        return nullptr;
        std::cout << "Error, bad syntax.\nFailed to parse after:\n\t";
        left->print();
    }

    result = stack->allocate<Divide>();
    result->left = left;
    result->right = right;

    return result;

}

Node* Parser::PNegative(){
    Node* right = nullptr;
    Negative* result = nullptr;

    if(!check("-")){
        return nullptr;
    }

    right = PNegative();
    if(right != nullptr){
        result = stack->allocate<Negative>();
        result->subexpression = right;
        return result;
    }

    right = PExponent();
    if(right != nullptr){
        result = stack->allocate<Negative>();
        result->subexpression = right;
        return result;
    }

    return nullptr;

}

Node* Parser::PExponent(){
    Node* left = nullptr;
    Node* right = nullptr;
    Exponent* result = nullptr;

    left = PFactorial();
    if(left == nullptr){
        return nullptr;
    } 

    if(!check("^")){
        return left;
    }

    right = PExponent();
    if(right == nullptr){
        return nullptr;
        std::cout << "Error, bad syntax.\nFailed to parse after:\n\t";
        left->print();
    }

    result = stack->allocate<Exponent>();
    result->left = left;
    result->right = right;

    return result;

}

Node* Parser::PFactorial(){
    Node* left = nullptr;
    Factorial* result = nullptr;

    left = PPrimitive();
    if(left == nullptr){
        return nullptr;
    }

    if(!check("!")){
        return left;
    }

    result = stack->allocate<Factorial>();
    result->left = left;

    return result;
}

Node* Parser::PPrimitive(){
    Primitive* result = nullptr;

    if(given[current].type == LiteralType::Fixed){
        result = stack->allocate<Primitive>();
        result->number.type = NumberType::fixed;
        result->number.number.fixed8[0] = std::stoll(std::string(given[current].name));
        current++;

        return result;
    }

    if(given[current].type == LiteralType::Floating){
        result = stack->allocate<Primitive>();
        result->number.type = NumberType::floating;
        result->number.number.float8[0] = std::stod(std::string(given[current].name));
        current++;

        return result;
    }

    if(check("e")){
        result = stack->allocate<Primitive>();
        result->number.type = NumberType::floating;
        result->type = NumberType::constant;
        result->number.number.float8[0] = 2.7182818284590452;

        return result;
    }

    if(check("pi")){
        result = stack->allocate<Primitive>();
        result->number.type = NumberType::floating;
        result->type = NumberType::constant;
        result->number.number.float8[0] = 3.1415926535897932;

        return result;
    }

    if(check("i")){
        result= stack->allocate<Primitive>();
        result->number.type = NumberType::complex;
        result->type = NumberType::constant;
        result->number.number.float8[0] = 0.0;
        result->number.number.float8[1] = 1.0;

        return result;
    }

    Node* newResult = PParentheses();
    if(newResult != nullptr){
        return newResult;
    }

    return nullptr;
}

Node* Parser::PParentheses(){
    Parentheses* result = nullptr;
    Node* subexpression = nullptr;
    if(!check("(")){
        return nullptr;
    }

    subexpression = PAdd();
    if(subexpression == nullptr){
        std::cout << "Error, bad parentheses subexpression.\n";
        return nullptr;
    }

    if(!check(")")){
        return nullptr;
    }

    result = stack->allocate<Parentheses>();
    result->subexpression = subexpression;

    return result;
}