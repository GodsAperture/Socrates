#include <iostream>
#include "Node.hpp"

Primitive::Primitive(){
    //Do nothing
}

void Primitive::print(){
    switch(type){
        case fixed:
            std::cout << number.fixed8[0];
            return;
        case fraction:
            std::cout << number.fixed8[0] << "/" << number.fixed8[1];
            return;
        case floating:
            std::cout << number.float8[0];
            return;
        case complex:
            std::cout << number.float8[0];
            if(number.float8[1] < 0){
                std::cout << number.float8[1] << "i";
            } else {
                std::cout << " + " << number.float8[1] << "i";
            }
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



Variable::Variable(){
    //Do nothing
}

void Variable::print(){
    std::cout << name;
}

Number Variable::evaluate(){
    return value;
}