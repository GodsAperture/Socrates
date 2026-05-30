#include <iostream>
#include "Lexer/Lexer.hpp"
#include "Objects/Parser.hpp"

int main(){
    std::string userInput = "";
    std::cout << "Input some text: ";
    std::getline(std::cin, userInput);

    Lexer EulerLexer = Lexer(userInput);
    std::vector<Token> tokenStream = EulerLexer.tokenize();

    Parser EulerParser;
    EulerParser.given = tokenStream;
    EulerParser.stack = new StackAllocator(2048);
    EulerParser.parse();

    EulerParser.AST->print();
    std::cout << "\n";
    EulerParser.AST->evaluate().print();
    std::cout << "\n";

    return 0;
}