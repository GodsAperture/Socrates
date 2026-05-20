#ifndef LEXER_h__

#include <string>
#include <vector>
#include "Token.hpp"

class Lexer{
public:
    int64_t pointer = 0;
    std::string theText = "";

    Lexer();
    Lexer(std::string input);
    std::vector<Token> tokenize();
    Token next();
};

#endif