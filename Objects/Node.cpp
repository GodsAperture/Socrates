#ifndef NODE_h__
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
#endif
