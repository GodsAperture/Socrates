#include <iostream>
#include "Lexer.hpp"

inline bool isOperator(char input){
    if(input == '+'){
        return true;
    }
    if(input == '-'){
        return true;
    }
    if(input == '*'){
        return true;
    }
    if(input == '/'){
        return true;
    }
    if(input == '%'){
        return true;
    }
    if(input == '^'){
        return true;
    }
    if(input == '('){
        return true;
    }
    if(input == ')'){
        return true;
    }
    if(input == '['){
        return true;
    }
    if(input == ']'){
        return true;
    }
    
    return false;
}

Lexer::Lexer(){
    //Do nothing
}

Lexer::Lexer(std::string input){
    theText = input;
}

Token Lexer::next(){
    Token result;
    char currentCharacter = theText[pointer];

    //Eliminate white space, if any.
    while(currentCharacter == ' '){ 
        pointer++;
        currentCharacter = theText[pointer];
    }

    //This is the starting point for the upcoming token.
    int64_t start = pointer;

    //Check for numbers
    if(isdigit(currentCharacter)){
        result.type = LiteralType::Fixed;
        //Check for a fixed point number.
        while(isdigit(currentCharacter)){
            pointer++;
            currentCharacter = theText[pointer];
        }

        //Check for a floating point number.
        if(currentCharacter == '.'){
            result.type = LiteralType::Floating;
            pointer++;
        } else {
            result.name = std::string_view(theText.data() + start, pointer - start);
            return result;
        }

        //Check for a mantissa.
        while(isdigit(currentCharacter)){
            pointer++;
            currentCharacter = theText[pointer];
        }

        result.name = std::string_view(theText.data() + start, pointer - start);
    }

    //Check for variables and special constants.
    if(isalpha(currentCharacter)){
        result.type = LiteralType::Variable;

        //Check for all the alphanumeric characters.
        while(isalnum(currentCharacter)){
            pointer++;
            currentCharacter = theText[pointer];
        }

        result.name = std::string_view(theText.data() + start, pointer - start);

        //Check to see if it is any of the special reserved characters.
        if(result.name == "pi"){
            result.type = LiteralType::Constant;
        }
        if(result.name == "e"){
            result.type = LiteralType::Constant;
        }
        if(result.name == "i"){
            result.type = LiteralType::Complex;
        }

        return result;
    }

    //Check for basic operators.
    //+, -, *, /, %, ^, (, ), [, ]
    if(isOperator(currentCharacter)){
        result.type = LiteralType::Operator;
        result.name = std::string_view(theText.data() + start, 1);
        pointer++;
        return result;
    }

    //Hopefully unreachable.
    return result;
}

std::vector<Token> Lexer::tokenize(){
    std::vector<Token> result;

    while(pointer < theText.length()){
        result.push_back(next());
    }
    
    return result;
}