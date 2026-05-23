#include <iostream>
#include "Lexer/Lexer.hpp"
#include "Objects/Parser.hpp"

int main(){
    std::string userInput = "";
    std::cout << "Input some text: ";
    std::getline(std::cin, userInput);

    Lexer EulerLexer = Lexer(userInput);
    std::vector<Token> tokenStream = EulerLexer.tokenize();

    Parser EulerParser = Parser(tokenStream);
    EulerParser.parse();

    std::cout << EulerParser.AST->evaluate().fixed8[0] << "\n";

    return 0;
}