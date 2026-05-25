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
            std::cout << number.number.fixed8[0];
            return;
        case NumberType::fraction:
            std::cout << number.number.fixed8[0] << "/" << number.number.fixed8[1];
            return;
        case NumberType::floating:
            std::cout << number.number.float8[0];
            return;
        case NumberType::complex:
            std::cout << number.number.float8[0];
            if(number.number.float8[1] < 0){
                std::cout << " - " << -number.number.float8[1] << "i";
            } else {
                std::cout << " + " << number.number.float8[1] << "i";
            }
            return;
        case NumberType::constant:
            if(number.number.float8[0] == 3.1415926535897932){
                std::cout << "pi";
                return;
            }
            if(number.number.float8[0] == 2.7182818284590452){
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



Imaginary::Imaginary(){
    //Do nothing
}

void Imaginary::print(){
    coefficient->print();
    std::cout << "i";
}

Number Imaginary::evaluate(){
    Number result;
    result.type = NumberType::complex;
    Number evaluation = coefficient->evaluate();

    switch(evaluation.type){
        case NumberType::fixed:
            result.number.float8[1] = ((double) evaluation.number.fixed8[0]);
            return result;
        case NumberType::fraction:
            result.number.float8[1] = ((double) evaluation.number.fixed8[0]) / ((double) evaluation.number.fixed8[1]);
            return result;
        case NumberType::floating:
            result.number.float8[1] = evaluation.number.float8[0];
            return result;
        case NumberType::complex:
            result.number.float8[0] = -evaluation.number.float8[1];
            result.number.float8[1] = evaluation.number.float8[0];
            return result;
        default:
            return result;
    }
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

Number Exponent::evaluate(){
    return left->evaluate() ^ right->evaluate();
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



AbsoluteValue::AbsoluteValue(){
    //Do nothing
}

void AbsoluteValue::print(){
    std::cout << "|";
    subexpression->print();
    std::cout << "|";
}

Number AbsoluteValue::evaluate(){
    Number result = subexpression->evaluate();

    switch(result.type){
        case NumberType::fixed:
            if(result.number.fixed8[0] < 0){
                return -result;
            } else {
                return result;
            }
        case NumberType::fraction:
            if(result.number.fixed8[0] < 0){
                result.number.fixed8[0] = -result.number.fixed8[0];
            }
            if(result.number.fixed8[1] < 0){
                result.number.fixed8[1] = -result.number.fixed8[1];
            }
            return result;
        case NumberType::floating:
            if(result.number.float8[0] < 0){
                return -result;
            }
        case NumberType::complex:
            result.type = NumberType::floating;
            result.number.float8[0] = sqrt(result.number.float8[0] * result.number.float8[0] + result.number.float8[1] * result.number.float8[1]);
            result.number.float8[1] = 0.0;
            return result;
        default:
            return result;
    }
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