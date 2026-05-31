#include <iostream>
#include "Lexer/Lexer.hpp"
#include "Objects/Parser.hpp"

int main(){
    std::string userInput = "";
    std::cout << "Input some text: ";
    std::getline(std::cin, userInput);
    Duo<bool, Node*> result;

    Lexer EulerLexer = Lexer(userInput);
    std::vector<Token> tokenStream = EulerLexer.tokenize();

    Parser EulerParser;
    EulerParser.given = tokenStream;
    EulerParser.stack = new StackAllocator(2048);
    EulerParser.parse();
    result.first = false;
    result.second = EulerParser.AST;

    EulerParser.AST->print();
    std::cout << "\n\nSuggested order of execution:\n";
    while(!result.first){
        result.second->stepPrint();
        std::cout << "\n";
        result = result.second->step(EulerParser.stack);
    }
    std::cout << "\n";
    EulerParser.AST->evaluate().print();
    std::cout << "\n";

    return 0;
}