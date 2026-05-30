#ifndef TYPES_c__
#define TYPES_c__
#include <cstdint>
#include <math.h>
#include <iostream>

enum NumberType : uint8_t{
    fixed,
    fraction,
    floating,
    complex,
    constant
};

/// @brief For use identifying types by the lexer.
enum LiteralType : uint8_t{
    LITERALNULLTYPE,
    Fixed,
    Floating,
    Complex,
    LiteralVariable,
    Operator,
    Constant
};

union EmulatedNumber{
    //0 is real, 1 is imaginary.
    double float8[2];

    //0 is numerator, 1 is denominator. 
    int64_t fixed8[2];
};

class Number{
public:
    NumberType type;
    EmulatedNumber number;

    Number(){
        number.fixed8[0] = 0;
        number.fixed8[1] = 0;
    }

        bool operator==(Number right){
        return (number.fixed8[0] == right.number.fixed8[0]) && (number.fixed8[1] == right.number.fixed8[1]);
    }

    //This will be used to approximate floating point numbers to ~4 decimal places.
    //For specific rounding when the user is prompted to return an answer to 4 decimal places.
    inline Number mask4(Number input){
        input.number.fixed8[0] = 0xFFFFFF8000000000 & input.number.fixed8[0];
        return input;
    }

    //This will be used to approximate floating point numbers to ~12 decimal places.
    //This is for general purpose equivalency checks.
    inline Number mask12(Number input){
        input.number.fixed8[0] = 0xffffffffffff0000 & input.number.fixed8[0];
        return input;
    }

    Number operator=(Number right){
        type = right.type;
        number.fixed8[0] = right.number.fixed8[0];
        number.fixed8[1] = right.number.fixed8[1];

        return *this;
    }

    Number operator+(Number right){
        Number left = *this;
        Number result;

        switch(type){
            case NumberType::fixed:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::fixed;
                        result.number.fixed8[0] = left.number.fixed8[0] + right.number.fixed8[0];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::fraction;
                        result.number.fixed8[0] = left.number.fixed8[0] * right.number.fixed8[1] + right.number.fixed8[0];
                        result.number.fixed8[1] = right.number.fixed8[1];
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.number.float8[0] = ((double) left.number.fixed8[0]) + right.number.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = ((double) left.number.fixed8[0]) + right.number.float8[0];
                        result.number.float8[1] = right.number.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::fraction:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::fraction;
                        result.number.fixed8[0] = left.number.fixed8[0] + right.number.fixed8[0] * left.number.fixed8[1];
                        result.number.fixed8[1] = left.number.fixed8[1];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::fraction;
                        result.number.fixed8[0] = left.number.fixed8[0] * right.number.fixed8[1] + left.number.fixed8[1] * right.number.fixed8[0];
                        result.number.fixed8[1] = left.number.fixed8[1] * right.number.fixed8[1];

                        //Determine the lowest common denominator of the fraction, if one exists.
                        if(result.number.fixed8[0] % result.number.fixed8[1] == 0){
                            int64_t numerator = result.number.fixed8[0];
                            int64_t denominator = result.number.fixed8[1];
                            int64_t remainder = 0;

                            while(remainder != 0){
                                remainder = denominator % remainder;

                                numerator = denominator;
                                denominator = remainder;
                            }

                            result.number.fixed8[0] = result.number.fixed8[0] / remainder;
                            result.number.fixed8[1] = result.number.fixed8[1] / remainder;
                        }

                        //If the denominator is 1, then it's no longer a fraction.
                        if(result.number.fixed8[1] == 1){
                            result.type = NumberType::fixed;
                            result.number.fixed8[1] = 0;
                        }

                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.number.fixed8[0] = ((double) left.number.fixed8[0]) / ((double) left.number.fixed8[1]) + right.number.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = ((double) left.number.fixed8[0]) / ((double) left.number.fixed8[1]) + right.number.float8[0];
                        result.number.float8[1] = right.number.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::floating:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::floating;
                        result.number.float8[0] = left.number.float8[0] + ((double) right.number.fixed8[0]);
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::floating;
                        result.number.float8[0] = left.number.float8[0] + ((double) right.number.fixed8[0]) / ((double) right.number.fixed8[1]);
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.number.float8[0] = left.number.float8[0] + right.number.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] + right.number.float8[0];
                        result.number.float8[1] = right.number.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::complex:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] + ((double) right.number.fixed8[0]);
                        result.number.float8[1] = left.number.float8[1];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] + ((double) right.number.fixed8[0]) / ((double) right.number.fixed8[1]);
                        result.number.float8[1] = left.number.float8[1];
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] + right.number.float8[0];
                        result.number.float8[1] = left.number.float8[1];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] + right.number.float8[0];
                        result.number.float8[1] = left.number.float8[1] + right.number.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            default:
                //Unreachable.
                return result;
        }
    }

    Number operator-(Number right){
        Number left = *this;
        Number result;

        switch(this->type){
            case NumberType::fixed:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::fixed;
                        result.number.fixed8[0] = left.number.fixed8[0] - right.number.fixed8[0];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::fraction;
                        result.number.fixed8[0] = left.number.fixed8[0] * right.number.fixed8[1] - right.number.fixed8[0];
                        result.number.fixed8[1] = right.number.fixed8[1];
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.number.float8[0] = ((double) left.number.fixed8[0]) - right.number.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = ((double) left.number.fixed8[0]) - right.number.float8[0];
                        result.number.float8[1] = right.number.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::fraction:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::fraction;
                        result.number.fixed8[0] = left.number.fixed8[0] - right.number.fixed8[0] * left.number.fixed8[1];
                        result.number.fixed8[1] = left.number.fixed8[1];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::fraction;
                        result.number.fixed8[0] = left.number.fixed8[0] * right.number.fixed8[1] - left.number.fixed8[1] * right.number.fixed8[0];
                        result.number.fixed8[1] = left.number.fixed8[1] * right.number.fixed8[1];

                        //Determine the lowest common denominator of the fraction, if one exists.
                        if(result.number.fixed8[0] % result.number.fixed8[1] == 0){
                            int64_t numerator = result.number.fixed8[0];
                            int64_t denominator = result.number.fixed8[1];
                            int64_t remainder = 0;

                            while(remainder != 0){
                                remainder = denominator % remainder;

                                numerator = denominator;
                                denominator = remainder;
                            }

                            result.number.fixed8[0] = result.number.fixed8[0] / remainder;
                            result.number.fixed8[1] = result.number.fixed8[1] / remainder;
                        }

                        //If the denominator is 1, then it's no longer a fraction.
                        if(result.number.fixed8[1] == 1){
                            result.type = NumberType::fixed;
                            result.number.fixed8[1] = 0;
                        }

                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.number.fixed8[0] = ((double) left.number.fixed8[0]) / ((double) left.number.fixed8[1]) - right.number.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = ((double) left.number.fixed8[0]) / ((double) left.number.fixed8[1]) - right.number.float8[0];
                        result.number.float8[1] = -right.number.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::floating:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::floating;
                        result.number.float8[0] = left.number.float8[0] - ((double) right.number.fixed8[0]);
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::floating;
                        result.number.float8[0] = left.number.float8[0] - ((double) right.number.fixed8[0]) / ((double) right.number.fixed8[1]);
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.number.float8[0] = left.number.float8[0] - right.number.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] - right.number.float8[0];
                        result.number.float8[1] = -right.number.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::complex:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] - ((double) right.number.fixed8[0]);
                        result.number.float8[1] = left.number.float8[1];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] - ((double) right.number.fixed8[0]) / ((double) right.number.fixed8[1]);
                        result.number.float8[1] = left.number.float8[1];
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] - right.number.float8[0];
                        result.number.float8[1] = left.number.float8[1];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] - right.number.float8[0];
                        result.number.float8[1] = left.number.float8[1] - right.number.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            default:
                //Unreachable.
                return result;
        }
    }

    Number operator-(){
        switch(type){
            case fixed:
                this->number.fixed8[0] = -this->number.fixed8[0];
                return *this;
            case fraction:
                this->number.fixed8[0] = -this->number.fixed8[0];
                return *this;
            case floating:
                this->number.float8[0] = -this->number.float8[0];
                return *this;
            case complex:
                this->number.float8[0] = -this->number.float8[0];
                this->number.float8[1] = -this->number.float8[1];
                return *this;
            default:
                return *this;
        }
    }

    Number operator*(Number right){
        Number left = *this;
        Number result;
        switch(this->type){
            case NumberType::fixed:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::fixed;
                        result.number.fixed8[0] = left.number.fixed8[0] * right.number.fixed8[0];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::fraction;
                        result.number.fixed8[0] = left.number.fixed8[0] * right.number.fixed8[1];
                        result.number.fixed8[1] = left.number.fixed8[1] * right.number.fixed8[0];

                        //Determine the lowest common denominator of the fraction, if one exists.
                        if(result.number.fixed8[0] % result.number.fixed8[1] == 0){
                            int64_t numerator = result.number.fixed8[0];
                            int64_t denominator = result.number.fixed8[1];
                            int64_t remainder = 0;

                            while(remainder != 0){
                                remainder = denominator % remainder;

                                numerator = denominator;
                                denominator = remainder;
                            }

                            result.number.fixed8[0] = result.number.fixed8[0] / remainder;
                            result.number.fixed8[1] = result.number.fixed8[1] / remainder;
                        }

                        //If the denominator is 1, then it's no longer a fraction.
                        if(result.number.fixed8[1] == 1){
                            result.type = NumberType::fixed;
                            result.number.fixed8[1] = 0;
                        }

                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.number.float8[0] = ((double) left.number.fixed8[0]) * right.number.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = ((double) left.number.fixed8[0]) * right.number.float8[0];
                        result.number.float8[1] = ((double) left.number.fixed8[0]) * right.number.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::fraction:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::fraction;
                        result.number.fixed8[0] = left.number.fixed8[0] * right.number.fixed8[0];
                        result.number.fixed8[1] = left.number.fixed8[1];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::fraction;
                        result.number.fixed8[0] = left.number.fixed8[0] * right.number.fixed8[0];
                        result.number.fixed8[1] = left.number.fixed8[1] * right.number.fixed8[0];

                        //Determine the lowest common denominator of the fraction, if one exists.
                        if(result.number.fixed8[0] % result.number.fixed8[1] == 0){
                            int64_t numerator = result.number.fixed8[0];
                            int64_t denominator = result.number.fixed8[1];
                            int64_t remainder = 0;

                            while(remainder != 0){
                                remainder = denominator % remainder;

                                numerator = denominator;
                                denominator = remainder;
                            }

                            result.number.fixed8[0] = result.number.fixed8[0] / remainder;
                            result.number.fixed8[1] = result.number.fixed8[1] / remainder;
                        }

                        //If the denominator is 1, then it's no longer a fraction.
                        if(result.number.fixed8[1] == 1){
                            result.type = NumberType::fixed;
                            result.number.fixed8[1] = 0;
                        }

                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.number.fixed8[0] = ((double) left.number.fixed8[0]) * right.number.float8[0] / ((double) left.number.fixed8[1]);
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = ((double) left.number.fixed8[0]) * right.number.float8[0] / ((double) left.number.fixed8[1]);
                        result.number.float8[1] = ((double) left.number.fixed8[0]) * right.number.float8[0] / ((double) left.number.fixed8[1]);
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::floating:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::floating;
                        result.number.float8[0] = left.number.float8[0] * ((double) right.number.fixed8[0]);
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::floating;
                        result.number.float8[0] = left.number.float8[0] * ((double) right.number.fixed8[0]) / ((double) right.number.fixed8[1]);
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.number.float8[0] = left.number.float8[0] * right.number.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] * right.number.float8[0];
                        result.number.float8[1] = left.number.float8[0] * right.number.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::complex:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] * ((double) right.number.fixed8[0]);
                        result.number.float8[1] = left.number.float8[1] * ((double) right.number.fixed8[0]);
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] * ((double) right.number.fixed8[0]) / ((double) right.number.fixed8[1]);
                        result.number.float8[1] = left.number.float8[1] * ((double) right.number.fixed8[0]) / ((double) right.number.fixed8[1]);
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] * right.number.float8[0];
                        result.number.float8[1] = left.number.float8[1] * right.number.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] * right.number.float8[0] - left.number.float8[1] * right.number.float8[1];
                        result.number.float8[1] = left.number.float8[0] * right.number.float8[1] + left.number.float8[1] * right.number.float8[0];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            default:
                //Unreachable.
                return result;
        }
    }

    Number operator/(Number right){
        Number left = *this;
        Number result;
        switch(this->type){
            case NumberType::fixed:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::fraction;
                        result.number.fixed8[0] = left.number.fixed8[0];
                        result.number.fixed8[1] = right.number.fixed8[0];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::fraction;
                        result.number.fixed8[0] = left.number.fixed8[0] * right.number.fixed8[1];
                        result.number.fixed8[1] = right.number.fixed8[0];

                        //Determine the lowest common denominator of the fraction, if one exists.
                        if(result.number.fixed8[0] % result.number.fixed8[1] == 0){
                            int64_t numerator = result.number.fixed8[0];
                            int64_t denominator = result.number.fixed8[1];
                            int64_t remainder = 0;

                            while(remainder != 0){
                                remainder = denominator % remainder;

                                numerator = denominator;
                                denominator = remainder;
                            }

                            result.number.fixed8[0] = result.number.fixed8[0] / remainder;
                            result.number.fixed8[1] = result.number.fixed8[1] / remainder;
                        }

                        //If the denominator is 1, then it's no longer a fraction.
                        if(result.number.fixed8[1] == 1){
                            result.type = NumberType::fixed;
                            result.number.fixed8[1] = 0;
                        }

                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.number.float8[0] = ((double) left.number.fixed8[0]) / right.number.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = ((double) left.number.fixed8[0]) * right.number.float8[0] / (right.number.float8[0] * right.number.float8[0] + right.number.float8[1] * right.number.float8[1]);
                        result.number.float8[1] = -((double) left.number.fixed8[0]) * right.number.float8[1] / (right.number.float8[0] * right.number.float8[0] + right.number.float8[1] * right.number.float8[1]);
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::fraction:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::fraction;
                        result.number.fixed8[0] = left.number.fixed8[0];
                        result.number.fixed8[1] = left.number.fixed8[1] * right.number.fixed8[1];

                        //Determine the lowest common denominator of the fraction, if one exists.
                        if(result.number.fixed8[0] % result.number.fixed8[1] == 0){
                            int64_t numerator = result.number.fixed8[0];
                            int64_t denominator = result.number.fixed8[1];
                            int64_t remainder = 0;

                            while(remainder != 0){
                                remainder = denominator % remainder;

                                numerator = denominator;
                                denominator = remainder;
                            }

                            result.number.fixed8[0] = result.number.fixed8[0] / remainder;
                            result.number.fixed8[1] = result.number.fixed8[1] / remainder;
                        }

                        //If the denominator is 1, then it's no longer a fraction.
                        if(result.number.fixed8[1] == 1){
                            result.type = NumberType::fixed;
                            result.number.fixed8[1] = 0;
                        }

                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::fraction;
                        result.number.fixed8[0] = left.number.fixed8[0] * right.number.fixed8[1];
                        result.number.fixed8[1] = left.number.fixed8[1] * right.number.fixed8[0];

                        //Determine the lowest common denominator of the fraction, if one exists.
                        if(result.number.fixed8[0] % result.number.fixed8[1] == 0){
                            int64_t numerator = result.number.fixed8[0];
                            int64_t denominator = result.number.fixed8[1];
                            int64_t remainder = 0;

                            while(remainder != 0){
                                remainder = denominator % remainder;

                                numerator = denominator;
                                denominator = remainder;
                            }

                            result.number.fixed8[0] = result.number.fixed8[0] / remainder;
                            result.number.fixed8[1] = result.number.fixed8[1] / remainder;
                        }

                        //If the denominator is 1, then it's no longer a fraction.
                        if(result.number.fixed8[1] == 1){
                            result.type = NumberType::fixed;
                            result.number.fixed8[1] = 0;
                        }

                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.number.fixed8[0] = ((double) left.number.fixed8[1]) / ((double) left.number.fixed8[0]) * right.number.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = ((double) left.number.fixed8[1]) / ((double) left.number.fixed8[0]) * right.number.float8[0];
                        result.number.float8[1] = ((double) left.number.fixed8[1]) / ((double) left.number.fixed8[0]) * right.number.float8[0];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::floating:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::floating;
                        result.number.float8[0] = left.number.float8[0] / ((double) right.number.fixed8[0]);
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::floating;
                        result.number.float8[0] = left.number.float8[0] * ((double) right.number.fixed8[1]) / ((double) right.number.fixed8[0]);
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.number.float8[0] = left.number.float8[0] / right.number.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] * right.number.float8[0] / (right.number.float8[0] * right.number.float8[0] + right.number.float8[1] * right.number.float8[1]);
                        result.number.float8[1] = -left.number.float8[1] * right.number.float8[1] / (right.number.float8[0] * right.number.float8[0] + right.number.float8[1] * right.number.float8[1]);
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::complex:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] / ((double) right.number.fixed8[0]);
                        result.number.float8[1] = left.number.float8[1] / ((double) right.number.fixed8[0]);
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] * ((double) right.number.fixed8[1]) / ((double) right.number.fixed8[0]);
                        result.number.float8[1] = left.number.float8[1] * ((double) right.number.fixed8[1]) / ((double) right.number.fixed8[0]);
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::complex;
                        result.number.float8[0] = left.number.float8[0] / right.number.float8[0];
                        result.number.float8[1] = left.number.float8[1] / right.number.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = (left.number.float8[0] * right.number.float8[0] + left.number.float8[1] * right.number.float8[1]) / (right.number.float8[0] * right.number.float8[0] + right.number.float8[1] * right.number.float8[1]);
                        result.number.float8[1] = (right.number.float8[0] * left.number.float8[1] - right.number.float8[1] * left.number.float8[0]) / (right.number.float8[0] * right.number.float8[0] + right.number.float8[1] * right.number.float8[1]);
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            default:
                //Unreachable.
                return result;
        }
    }

    Number operator^(Number right){
        //This is only for complex left types.
        double logMagnitude = 0.5 * log(number.float8[0] * number.float8[0] + number.float8[1] * number.float8[1]);
        double angle = atan2(number.float8[1], number.float8[0]);
        //For the cases when the right is indeed a fixed8.
        //Otherwise it's ignorable trash.
        int64_t remainingExponent = right.number.fixed8[0];
        int64_t bitmask = 1;
        bool isNegative;
        Number result;
        //A handy variable for generating the result from the base.
        Number handy;
        result.number.fixed8[0] = 1;

        switch(type){
            case NumberType::fixed:
                switch(right.type){
                    //The algorithm here is me abusing exponential growth.
                    //Each bit in the exponnet tells me which exponentials of the base to use.
                    case NumberType::fixed:
                        result.type = NumberType::fixed;
                        handy = *this;
                        isNegative = right.number.fixed8[0] < 0;

                        if(isNegative){
                            right.number.fixed8[0] = -right.number.fixed8[0];
                        }

                        while(remainingExponent != 0){
                            //If the bit is 0, then I don't want to multiply the result.
                            if((bitmask & remainingExponent) == 1){
                                result = result * handy;
                            }
                            handy = handy * handy;
                            remainingExponent = remainingExponent >> 1;
                        }

                        //If the result is negative, then it is to be reciprocated.
                        if(isNegative){
                            result.type = NumberType::fraction;
                            result.number.fixed8[1] = result.number.fixed8[0];
                            result.number.fixed8[0] = 1;
                        }
                        return result;
                    case NumberType::fraction:
                        //A fractional exponent will always yield a floating point value.
                        //Leaving it as a fractional exponent will just complicate the math a lot for me.
                        result.type = NumberType::floating;
                        result.number.float8[0] = pow((double) this->number.fixed8[0], ((double) right.number.fixed8[0]) / ((double) right.number.fixed8[1]));
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.number.float8[0] = pow((double) this->number.fixed8[0], right.number.float8[0]);
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = exp(log((double) number.fixed8[0]) * right.number.float8[0]) * cos(((double) number.fixed8[0]) * right.number.float8[1]);
                        result.number.float8[1] = exp(log((double) number.fixed8[0]) * right.number.float8[0]) * sin(((double) number.fixed8[0]) * right.number.float8[1]);
                        return result;
                    default:
                        //Unreachable
                        return result;
                }
            case NumberType::fraction:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::fraction;
                        isNegative = right.number.fixed8[0] < 0;

                        if(isNegative){
                            right.number.fixed8[0] = -right.number.fixed8[0];
                        }

                        while(remainingExponent != 0){
                            //If the bit is 0, then I don't want to multiply the result.
                            if((bitmask & remainingExponent) == 1){
                                result = result * handy;
                            }

                            handy = handy * handy;
                            remainingExponent = remainingExponent >> 1;
                        }

                        if(isNegative){
                            handy.number.fixed8[0] = result.number.fixed8[0];
                            result.number.fixed8[0] = result.number.fixed8[1];
                            result.number.fixed8[1] = handy.number.fixed8[0];
                        }

                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::floating;
                        result.number.float8[0] = pow(((double) number.fixed8[0]) / ((double) number.fixed8[1]), ((double) right.number.fixed8[0]) / ((double) right.number.fixed8[1]));
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.number.float8[0] = pow(((double) number.fixed8[0]) / ((double) number.fixed8[1]), right.number.float8[0]);
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = exp(log(((double) number.fixed8[0]) / ((double) number.fixed8[1])) * right.number.float8[0]) * cos(log(((double) number.fixed8[0]) / ((double) number.fixed8[1])) * right.number.float8[1]);
                        result.number.float8[0] = exp(log(((double) number.fixed8[0]) / ((double) number.fixed8[1])) * right.number.float8[0]) * sin(log(((double) number.fixed8[0]) / ((double) number.fixed8[1])) * right.number.float8[1]);
                        return result;
                    default:
                        //Unreachable
                        return result;
                }
            case NumberType::floating:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::floating;
                        result.number.float8[0] = pow(number.float8[0], (double) right.number.fixed8[0]);
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::floating;
                        result.number.float8[0] = pow(number.float8[0], ((double) right.number.fixed8[0]) / ((double) right.number.fixed8[1]));
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        //Edge case: The base is negative.
                        if(number.float8[0] < 0){
                            result.type = NumberType::complex;
                            result.number.float8[0] = pow(number.float8[0], right.number.float8[0]) * cos(3.14159265358979323846 * right.number.float8[0]);
                            result.number.float8[1] = pow(number.float8[0], right.number.float8[0]) * sin(3.14159265358979323846 * right.number.float8[0]);
                            return result;
                        }
                        result.number.float8[0] = pow(number.float8[0], right.number.float8[0]);
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = exp(log(number.float8[0]) * right.number.float8[0]) * cos(log(number.float8[0]) * right.number.float8[1]);
                        result.number.float8[1] = exp(log(number.float8[0]) * right.number.float8[0]) * sin(log(number.float8[0]) * right.number.float8[1]);
                        return result;
                    default:
                        //Unreachable
                        return result;
                }
            case NumberType::complex:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::complex;
                        result.number.float8[0] = exp(right.number.fixed8[0] * logMagnitude) * cos(right.number.fixed8[0] * angle);
                        result.number.float8[1] = exp(right.number.fixed8[0] * logMagnitude) * sin(right.number.float8[0] * angle);
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::complex;
                        result.number.float8[0] = exp(((double) right.number.fixed8[0]) / ((double) right.number.fixed8[1]) * logMagnitude) * cos(angle * ((double) right.number.fixed8[0]) / ((double) right.number.fixed8[1]));
                        result.number.float8[1] = exp(((double) right.number.fixed8[0]) / ((double) right.number.fixed8[1]) * logMagnitude) * sin(angle * ((double) right.number.fixed8[0]) / ((double) right.number.fixed8[1]));
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::complex;
                        result.number.float8[0] = exp(right.number.float8[0] * logMagnitude) * cos(angle * right.number.float8[0]);
                        result.number.float8[1] = exp(right.number.float8[0] * logMagnitude) * sin(angle * right.number.float8[0]);
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.number.float8[0] = exp(logMagnitude * right.number.float8[0] - angle * right.number.float8[1]) * cos(logMagnitude * right.number.float8[1] + angle * right.number.float8[0]);
                        result.number.float8[1] = exp(logMagnitude * right.number.float8[0] - angle * right.number.float8[1]) * sin(logMagnitude * right.number.float8[1] + angle * right.number.float8[0]);
                        return result;
                    default:
                        //Unreachable
                        return result;
                }
        }

        return result;
    }

    void print(){
            switch(type){
        case NumberType::fixed:
            std::cout << number.fixed8[0];
            return;
        case NumberType::fraction:
            std::cout << number.fixed8[0] << "/" << number.fixed8[1];
            return;
        case NumberType::floating:
            std::cout << number.float8[0];
            return;
        case NumberType::complex:
            std::cout << number.float8[0];
            if(number.float8[1] < 0){
                std::cout << " - " << -number.float8[1] << "i";
            } else {
                std::cout << " + " << number.float8[1] << "i";
            }
            return;
        case NumberType::constant:
            if(number.float8[0] == 3.1415926535897932){
                std::cout << "pi";
                return;
            }
            if(number.float8[0] == 2.7182818284590452){
                std::cout << "e";
                return;
            }
            std::cout << "i";
            return;
        default:
            return;
        }
    }
};

#endif