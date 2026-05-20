#ifndef TOKEN_h__
#include <string>
#include "../Objects/Types.cpp"

class Token{
public:
    std::string_view name;
    LiteralType type = LiteralType::LITERALNULLTYPE;

    Token();
};
#endif