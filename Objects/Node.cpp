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
            if(number.number.float8[0] == 3.1415926535897932){
                std::cout << "pi";
                return;
            }
            if(number.number.float8[0] == 2.7182818284590452){
                std::cout << "e";
                return;
            }
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
            std::cout << "Unknown number printed: " << (number.type + 0);
            return;
    }
}

bool Primitive::isEnd(){
    return true;
}

Duo<bool, Node*> Primitive::step(StackAllocator* stack){
    Duo<bool, Node*> result;
    Primitive* node = stack->allocate<Primitive>();
    node->number = number;
    node->type = number.type;

    result.first = true;
    result.second = node;

    return result;
}

void Primitive::stepPrint(){
    //Easy reuse of code.
    print();
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

bool Imaginary::isEnd(){
    return true;
}

Duo<bool, Node*> Imaginary::step(StackAllocator* stack){
    Duo<bool, Node*> result;
    Primitive* node = stack->allocate<Primitive>();
    node->number = evaluate();

    result.first = true;
    result.second = stack->allocate<Primitive>();

    return result;
}

void Imaginary::stepPrint(){
    if(coefficient->isEnd()){
        std::cout << ">> ";
        coefficient->stepPrint();
        std::cout << "i <<";
    } else {
        coefficient->stepPrint();
        std::cout << "i";
    }
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

bool Add::isEnd(){
    return false;
}

Duo<bool, Node*> Add::step(StackAllocator* stack){
    Duo<bool, Node*> result;
    Duo<bool, Node*> leftEvaluation = left->step(stack);
    Duo<bool, Node*> rightEvaluation = right->step(stack);
    result.first = false;

    //Check to see if it should be evaluated.
    if(leftEvaluation.first & rightEvaluation.first){
        Primitive* node = stack->allocate<Primitive>();
        node->number = evaluate();
        node->type = node->number.type;
        result.second = node;
    } else {
        Add* node = stack->allocate<Add>();
        node->left = leftEvaluation.second;
        node->right = rightEvaluation.second;
        result.second = node;
    }

    return result;
}

void Add::stepPrint(){
    if(left->isEnd() & right->isEnd()){
        std::cout << ">> ";
        left->stepPrint();
        std::cout << " + ";
        right->stepPrint();
        std::cout << " <<";
    } else {
        left->stepPrint();
        std::cout << " + ";
        right->stepPrint();
    }
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

bool Subtract::isEnd(){
    return false;
}

Duo<bool, Node*> Subtract::step(StackAllocator* stack){
    Duo<bool, Node*> result;
    Duo<bool, Node*> leftEvaluation = left->step(stack);
    Duo<bool, Node*> rightEvaluation = right->step(stack);
    result.first = false;

    //Check to see if it should be evaluated.
    if(leftEvaluation.first & rightEvaluation.first){
        Primitive* node = stack->allocate<Primitive>();
        node->number = evaluate();
        node->type = node->number.type;
        result.second = node;
    } else {
        Subtract* node = stack->allocate<Subtract>();
        node->left = leftEvaluation.second;
        node->right = rightEvaluation.second;
        result.second = node;
    }

    return result;
}

void Subtract::stepPrint(){
    if(left->isEnd() & right->isEnd()){
        std::cout << ">> ";
        left->stepPrint();
        std::cout << " - ";
        right->stepPrint();
        std::cout << " <<";
    } else {
        left->stepPrint();
        std::cout << " - ";
        right->stepPrint();
    }
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

bool Multiply::isEnd(){
    return false;
}

Duo<bool, Node*> Multiply::step(StackAllocator* stack){
    Duo<bool, Node*> result;
    Duo<bool, Node*> leftEvaluation = left->step(stack);
    Duo<bool, Node*> rightEvaluation = right->step(stack);
    result.first = false;

    //Check to see if it should be evaluated.
    if(leftEvaluation.first & rightEvaluation.first){
        Primitive* node = stack->allocate<Primitive>();
        node->number = evaluate();
        node->type = node->number.type;
        result.second = node;
    } else {
        Multiply* node = stack->allocate<Multiply>();
        node->left = leftEvaluation.second;
        node->right = rightEvaluation.second;
        result.second = node;
    }

    return result;
}

void Multiply::stepPrint(){
    if(left->isEnd() & right->isEnd()){
        std::cout << ">> ";
        left->stepPrint();
        std::cout << " * ";
        right->stepPrint();
        std::cout << " <<";
    } else {
        left->stepPrint();
        std::cout << " * ";
        right->stepPrint();
    }
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

bool Divide::isEnd(){
    return false;
}

Duo<bool, Node*> Divide::step(StackAllocator* stack){
    Duo<bool, Node*> result;
    Duo<bool, Node*> leftEvaluation = left->step(stack);
    Duo<bool, Node*> rightEvaluation = right->step(stack);
    result.first = false;

    //Check to see if it should be evaluated.
    if(leftEvaluation.first & rightEvaluation.first){
        Primitive* node = stack->allocate<Primitive>();
        node->number = evaluate();
        node->type = node->number.type;
        result.second = node;
    } else {
        Divide* node = stack->allocate<Divide>();
        node->left = leftEvaluation.second;
        node->right = rightEvaluation.second;
        result.second = node;
    }

    return result;
}

void Divide::stepPrint(){
    if(left->isEnd() & right->isEnd()){
        std::cout << ">> ";
        left->stepPrint();
        std::cout << " / ";
        right->stepPrint();
        std::cout << " <<";
    } else {
        left->stepPrint();
        std::cout << " / ";
        right->stepPrint();
    }
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

bool Exponent::isEnd(){
    return false;
}

Duo<bool, Node*> Exponent::step(StackAllocator* stack){
    Duo<bool, Node*> result;
    Duo<bool, Node*> leftEvaluation = left->step(stack);
    Duo<bool, Node*> rightEvaluation = right->step(stack);
    result.first = false;

    //Check to see if it should be evaluated.
    if(leftEvaluation.first & rightEvaluation.first){
        Primitive* node = stack->allocate<Primitive>();
        node->number = evaluate();
        node->type = node->number.type;
        result.second = node;
    } else {
        Exponent* node = stack->allocate<Exponent>();
        node->left = leftEvaluation.second;
        node->right = rightEvaluation.second;
        result.second = node;
    }

    return result;
}

void Exponent::stepPrint(){
    if(left->isEnd() & right->isEnd()){
        std::cout << ">> ";
        left->stepPrint();
        std::cout << "^";
        right->stepPrint();
        std::cout << " <<";
    } else {
        left->stepPrint();
        std::cout << "^";
        right->stepPrint();
    }
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

bool Parentheses::isEnd(){
    //This is a special case.
    //Otherwise, I'd end up with dumb shit like `>> (3) <<`.
    return subexpression->isEnd();
}

Duo<bool, Node*> Parentheses::step(StackAllocator* stack){
    Duo<bool, Node*> result;
    Duo<bool, Node*> evaluation = subexpression->step(stack);
    result.first = false;

    //Check to see if it should be evaluated.
    if(evaluation.first){
        Primitive* node = stack->allocate<Primitive>();
        node->number = evaluate();
        node->type = node->number.type;
        result.second = node;
    } else {
        if(evaluation.second->isEnd()){
            Primitive* node = stack->allocate<Primitive>();
            node->number = evaluate();
            node->type = node->number.type;
            result.second = node;
            return result;
        }
        Parentheses* node = stack->allocate<Parentheses>();
        node->subexpression = evaluation.second;
        result.second = node;
    }

    return result;
}

void Parentheses::stepPrint(){
    if(subexpression->isEnd()){
        subexpression->stepPrint();
    } else {
        std::cout << "(";
        subexpression->stepPrint();
        std::cout << ")";
    }
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

bool Negative::isEnd(){
    //This is a special case.
    //Otherwise, I'd end up with dumb shit like `>> -3 <<`.
    return subexpression->isEnd();
}

Duo<bool, Node*> Negative::step(StackAllocator* stack){
    Duo<bool, Node*> result;
    Duo<bool, Node*> evaluation = subexpression->step(stack);
    result.first = false;

    //Check to see if it should be evaluated.
    if(evaluation.first){
        Primitive* node = stack->allocate<Primitive>();
        node->number = evaluate();
        node->type = node->number.type;
        result.second = node;
    } else {
        Factorial* node = stack->allocate<Factorial>();
        node->left = evaluation.second;
        result.second = node;
    }

    return result;
}

void Negative::stepPrint(){
    if(subexpression->isEnd()){
        std::cout << ">> ";
        subexpression->stepPrint();
        std::cout << " <<";
    } else {
        subexpression->stepPrint();
    }
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

bool Factorial::isEnd(){
    return false;
}

Duo<bool, Node*> Factorial::step(StackAllocator* stack){
    Duo<bool, Node*> result;
    Duo<bool, Node*> evaluation = left->step(stack);
    result.first = false;


    //Check to see if it should be evaluated.
    if(evaluation.first){
        Primitive* node = stack->allocate<Primitive>();
        node->number = evaluate();
        node->type = node->number.type;
        result.second = node;
    } else {
        Factorial* node = stack->allocate<Factorial>();
        node->left = evaluation.second;
        result.second = node;
    }

    return result;
}

void Factorial::stepPrint(){
    if(left->isEnd()){
        std::cout << ">> ";
        left->stepPrint();
        std::cout << " <<";
    } else {
        left->stepPrint();
    }
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

bool AbsoluteValue::isEnd(){
    return false;
}

Duo<bool, Node*> AbsoluteValue::step(StackAllocator* stack){
    Duo<bool, Node*> result;
    Duo<bool, Node*> evaluation = subexpression->step(stack);
    result.first = false;

    //Check to see if it should be evaluated.
    if(evaluation.first){
        Primitive* node = stack->allocate<Primitive>();
        node->number = evaluate();
        node->type = node->number.type;
        result.second = node;
    } else {
        AbsoluteValue* node = stack->allocate<AbsoluteValue>();
        node->subexpression = evaluation.second;
        result.second = node;
    }

    return result;
}

void AbsoluteValue::stepPrint(){
    if(subexpression->isEnd()){
        std::cout << ">> ";
        subexpression->stepPrint();
        std::cout << " <<";
    } else {
        subexpression->stepPrint();
    }
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

bool Variable::isEnd(){
    return true;
}

Duo<bool, Node*> Variable::step(StackAllocator* stack){
    Duo<bool, Node*> result;
    Primitive* node = stack->allocate<Primitive>();
    node->number = evaluate();
    node->type = node->number.type;

    result.first = true;
    result.second = node;

    return result;
}

void Variable::stepPrint(){
    std::cout << name;
}

Number Variable::evaluate(){
    return value;
}