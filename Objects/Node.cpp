#include <iostream>
#include "Node.hpp"

Node::Node(){
    //Do nothing
}

Primitive::Primitive(){
    //Do nothing
}

void Primitive::print(){
    switch(type){
        case NumberType::fixed:
            std::cout << number.fixed8[0];
            return;
        case NumberType::fraction:
            std::cout << number.fixed8[0] << "/" << number.fixed8[1];
            return;
        case NumberType::floating:
            std::cout << number.float8[0];
            return;
        case NumberType::complex:
            std::cout << number.float8[0];
            if(number.float8[1] < 0){
                std::cout << " - " << -number.float8[1] << "i";
            } else {
                std::cout << " + " << number.float8[1] << "i";
            }
            return;
        case NumberType::constant:
            if(number.float8[0] == 3.1415926535897932){
                std::cout << "pi";
                return;
            }
            if(number.float8[0] == 2.7182818284590452){
                std::cout << "e";
                return;
            }
            std::cout << "i";
            return;
        default:
            return;
    }
}

Number Primitive::evaluate(){
    return number;
}



Add::Add(){
    //Do nothing
}

void Add::print(){
    left->print();
    std::cout << " + ";
    right->print();
}

Number Add::evaluate(){
    return left->evaluate() + right->evaluate();
}



Subtract::Subtract(){
    //Do nothing
}

void Subtract::print(){
    left->print();
    std::cout << " - ";
    right->print();
}

Number Subtract::evaluate(){
    return left->evaluate() - right->evaluate();
}



QuietMultiply::QuietMultiply(){
    //Do nothing
}

void QuietMultiply::print(){
    left->print();
    std::cout << " ";
    right->print();
}

Number QuietMultiply::evaluate(){
    return left->evaluate() * right->evaluate();
}



Multiply::Multiply(){
    //Do nothing
}

void Multiply::print(){
    left->print();
    std::cout << " * ";
    right->print();
}

Number Multiply::evaluate(){
    return left->evaluate() * right->evaluate();
}



Divide::Divide(){
    //Do nothing
}

void Divide::print(){
    left->print();
    std::cout << " / ";
    right->print();
}

Number Divide::evaluate(){
    return left->evaluate() / right->evaluate();
}



Exponent::Exponent(){
    //Do nothing
}

void Exponent::print(){
    left->print();
    std::cout << "^";
    right->print();
}

//Placeholder. FIX THIS!
Number Exponent::evaluate(){
    return right->evaluate();
}



Parentheses::Parentheses(){
    //Do nothing
}

void Parentheses::print(){
    std::cout << "(";
    subexpression->print();
    std::cout << ")";
}

Number Parentheses::evaluate(){
    return subexpression->evaluate();
}



Negative::Negative(){
    //Do nothing
}

void Negative::print(){
    std::cout << "-";
    subexpression->print();
}

Number Negative::evaluate(){
    return -subexpression->evaluate();
}



Factorial::Factorial(){
    //Do nothing
}

void Factorial::print(){
    left->print();
    std::cout << "!";
}

Number Factorial::evaluate(){
    Number result;
    //TO DO
    //The mathematical definition of factorial is very problematic.
    return result;
}



Variable::Variable(){
    //Do nothing
}

void Variable::print(){
    std::cout << name;
}

Number Variable::evaluate(){
    return value;
}