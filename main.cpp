#include <iostream>
#include "Lexer/Lexer.hpp"

int main(){
    std::string userInput = "";
    std::cout << "Input some text: ";
    std::getline(std::cin, userInput);

    Lexer SocratesLexer = Lexer(userInput);
    std::vector<Token> tokenStream = SocratesLexer.tokenize();

    

    return 0;
}