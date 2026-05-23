#ifndef TYPES_c__
#define TYPES_c__
#include <cstdint>
#include <math.h>

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
    Variable,
    Operator,
    Constant
};

union Number{
    NumberType type;
    //0 is real, 1 is imaginary.
    double float8[2];

    //0 is numerator, 1 is denominator. 
    int64_t fixed8[2];

    Number(){
        fixed8[0] = 0;
        fixed8[1] = 0;
    }

    bool operator==(Number right){
        return (fixed8[0] == right.fixed8[0]) && (fixed8[1] == right.fixed8[1]);
    }

    //This will be used to approximate floating point numbers to ~4 decimal places.
    //For specific rounding when the user is prompted to return an answer to 4 decimal places.
    inline Number mask4(Number input){
        input.fixed8[0] = 0xFFFFFF8000000000 & input.fixed8[0];
        return input;
    }

    //This will be used to approximate floating point numbers to ~12 decimal places.
    //This is for general purpose equivalency checks.
    inline Number mask12(Number input){
        input.fixed8[0] = 0xffffffffffff0000 & input.fixed8[0];
        return input;
    }

    Number operator+(Number right){
        Number left = *this;
        Number result;

        switch(this->type){
            case NumberType::fixed:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::fixed;
                        result.fixed8[0] = left.fixed8[0] + right.fixed8[0];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::fraction;
                        result.fixed8[0] = left.fixed8[0] * right.fixed8[1] + right.fixed8[0];
                        result.fixed8[1] = right.fixed8[1];
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.float8[0] = ((double) left.fixed8[0]) + right.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = ((double) left.fixed8[0]) + right.float8[0];
                        result.float8[1] = right.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::fraction:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::fraction;
                        result.fixed8[0] = left.fixed8[0] + right.fixed8[0] * left.fixed8[1];
                        result.fixed8[1] = left.fixed8[1];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::fraction;
                        result.fixed8[0] = left.fixed8[0] * right.fixed8[1] + left.fixed8[1] * right.fixed8[0];
                        result.fixed8[1] = left.fixed8[1] * right.fixed8[1];

                        //Determine the lowest common denominator of the fraction, if one exists.
                        if(result.fixed8[0] % result.fixed8[1] == 0){
                            int64_t numerator = result.fixed8[0];
                            int64_t denominator = result.fixed8[1];
                            int64_t remainder = 0;

                            while(remainder != 0){
                                remainder = denominator % remainder;

                                numerator = denominator;
                                denominator = remainder;
                            }

                            result.fixed8[0] = result.fixed8[0] / remainder;
                            result.fixed8[1] = result.fixed8[1] / remainder;
                        }

                        //If the denominator is 1, then it's no longer a fraction.
                        if(result.fixed8[1] == 1){
                            result.type = NumberType::fixed;
                            result.fixed8[1] = 0;
                        }

                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.fixed8[0] = ((double) left.fixed8[0]) / ((double) left.fixed8[1]) + right.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = ((double) left.fixed8[0]) / ((double) left.fixed8[1]) + right.float8[0];
                        result.float8[1] = right.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::floating:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::floating;
                        result.float8[0] = left.float8[0] + ((double) right.fixed8[0]);
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::floating;
                        result.float8[0] = left.float8[0] + ((double) right.fixed8[0]) / ((double) right.fixed8[1]);
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.float8[0] = left.float8[0] + right.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] + right.float8[0];
                        result.float8[1] = right.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::complex:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] + ((double) right.fixed8[0]);
                        result.float8[1] = left.float8[1];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] + ((double) right.fixed8[0]) / ((double) right.fixed8[1]);
                        result.float8[1] = left.float8[1];
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] + right.float8[0];
                        result.float8[1] = left.float8[1];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] + right.float8[0];
                        result.float8[1] = left.float8[1] + right.float8[1];
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
                        result.fixed8[0] = left.fixed8[0] - right.fixed8[0];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::fraction;
                        result.fixed8[0] = left.fixed8[0] * right.fixed8[1] - right.fixed8[0];
                        result.fixed8[1] = right.fixed8[1];
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.float8[0] = ((double) left.fixed8[0]) - right.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = ((double) left.fixed8[0]) - right.float8[0];
                        result.float8[1] = right.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::fraction:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::fraction;
                        result.fixed8[0] = left.fixed8[0] - right.fixed8[0] * left.fixed8[1];
                        result.fixed8[1] = left.fixed8[1];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::fraction;
                        result.fixed8[0] = left.fixed8[0] * right.fixed8[1] - left.fixed8[1] * right.fixed8[0];
                        result.fixed8[1] = left.fixed8[1] * right.fixed8[1];

                        //Determine the lowest common denominator of the fraction, if one exists.
                        if(result.fixed8[0] % result.fixed8[1] == 0){
                            int64_t numerator = result.fixed8[0];
                            int64_t denominator = result.fixed8[1];
                            int64_t remainder = 0;

                            while(remainder != 0){
                                remainder = denominator % remainder;

                                numerator = denominator;
                                denominator = remainder;
                            }

                            result.fixed8[0] = result.fixed8[0] / remainder;
                            result.fixed8[1] = result.fixed8[1] / remainder;
                        }

                        //If the denominator is 1, then it's no longer a fraction.
                        if(result.fixed8[1] == 1){
                            result.type = NumberType::fixed;
                            result.fixed8[1] = 0;
                        }

                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.fixed8[0] = ((double) left.fixed8[0]) / ((double) left.fixed8[1]) - right.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = ((double) left.fixed8[0]) / ((double) left.fixed8[1]) - right.float8[0];
                        result.float8[1] = -right.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::floating:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::floating;
                        result.float8[0] = left.float8[0] - ((double) right.fixed8[0]);
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::floating;
                        result.float8[0] = left.float8[0] - ((double) right.fixed8[0]) / ((double) right.fixed8[1]);
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.float8[0] = left.float8[0] - right.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] - right.float8[0];
                        result.float8[1] = -right.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::complex:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] - ((double) right.fixed8[0]);
                        result.float8[1] = left.float8[1];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] - ((double) right.fixed8[0]) / ((double) right.fixed8[1]);
                        result.float8[1] = left.float8[1];
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] - right.float8[0];
                        result.float8[1] = left.float8[1];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] - right.float8[0];
                        result.float8[1] = left.float8[1] - right.float8[1];
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
                this->fixed8[0] = -this->fixed8[0];
                return *this;
            case fraction:
                this->fixed8[0] = -this->fixed8[0];
                return *this;
            case floating:
                this->float8[0] = -this->float8[0];
                return *this;
            case complex:
                this->float8[0] = -this->float8[0];
                this->float8[1] = -this->float8[1];
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
                        result.fixed8[0] = left.fixed8[0] * right.fixed8[0];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::fraction;
                        result.fixed8[0] = left.fixed8[0] * right.fixed8[1];
                        result.fixed8[1] = left.fixed8[1] * right.fixed8[0];

                        //Determine the lowest common denominator of the fraction, if one exists.
                        if(result.fixed8[0] % result.fixed8[1] == 0){
                            int64_t numerator = result.fixed8[0];
                            int64_t denominator = result.fixed8[1];
                            int64_t remainder = 0;

                            while(remainder != 0){
                                remainder = denominator % remainder;

                                numerator = denominator;
                                denominator = remainder;
                            }

                            result.fixed8[0] = result.fixed8[0] / remainder;
                            result.fixed8[1] = result.fixed8[1] / remainder;
                        }

                        //If the denominator is 1, then it's no longer a fraction.
                        if(result.fixed8[1] == 1){
                            result.type = NumberType::fixed;
                            result.fixed8[1] = 0;
                        }

                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.float8[0] = ((double) left.fixed8[0]) * right.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = ((double) left.fixed8[0]) * right.float8[0];
                        result.float8[1] = ((double) left.fixed8[0]) * right.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::fraction:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::fraction;
                        result.fixed8[0] = left.fixed8[0] * right.fixed8[0];
                        result.fixed8[1] = left.fixed8[1];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::fraction;
                        result.fixed8[0] = left.fixed8[0] * right.fixed8[0];
                        result.fixed8[1] = left.fixed8[1] * right.fixed8[0];

                        //Determine the lowest common denominator of the fraction, if one exists.
                        if(result.fixed8[0] % result.fixed8[1] == 0){
                            int64_t numerator = result.fixed8[0];
                            int64_t denominator = result.fixed8[1];
                            int64_t remainder = 0;

                            while(remainder != 0){
                                remainder = denominator % remainder;

                                numerator = denominator;
                                denominator = remainder;
                            }

                            result.fixed8[0] = result.fixed8[0] / remainder;
                            result.fixed8[1] = result.fixed8[1] / remainder;
                        }

                        //If the denominator is 1, then it's no longer a fraction.
                        if(result.fixed8[1] == 1){
                            result.type = NumberType::fixed;
                            result.fixed8[1] = 0;
                        }

                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.fixed8[0] = ((double) left.fixed8[0]) * right.float8[0] / ((double) left.fixed8[1]);
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = ((double) left.fixed8[0]) * right.float8[0] / ((double) left.fixed8[1]);
                        result.float8[1] = ((double) left.fixed8[0]) * right.float8[0] / ((double) left.fixed8[1]);
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::floating:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::floating;
                        result.float8[0] = left.float8[0] * ((double) right.fixed8[0]);
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::floating;
                        result.float8[0] = left.float8[0] * ((double) right.fixed8[0]) / ((double) right.fixed8[1]);
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.float8[0] = left.float8[0] * right.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] * right.float8[0];
                        result.float8[1] = left.float8[0] * right.float8[1];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::complex:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] * ((double) right.fixed8[0]);
                        result.float8[1] = left.float8[1] * ((double) right.fixed8[0]);
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] * ((double) right.fixed8[0]) / ((double) right.fixed8[1]);
                        result.float8[1] = left.float8[1] * ((double) right.fixed8[0]) / ((double) right.fixed8[1]);
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] * right.float8[0];
                        result.float8[1] = left.float8[1] * right.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] * right.float8[0] - left.float8[1] * right.float8[1];
                        result.float8[1] = left.float8[0] * right.float8[1] + left.float8[1] * right.float8[0];
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
                        result.fixed8[0] = left.fixed8[0];
                        result.fixed8[1] = right.fixed8[0];
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::fraction;
                        result.fixed8[0] = left.fixed8[0] * right.fixed8[1];
                        result.fixed8[1] = right.fixed8[0];

                        //Determine the lowest common denominator of the fraction, if one exists.
                        if(result.fixed8[0] % result.fixed8[1] == 0){
                            int64_t numerator = result.fixed8[0];
                            int64_t denominator = result.fixed8[1];
                            int64_t remainder = 0;

                            while(remainder != 0){
                                remainder = denominator % remainder;

                                numerator = denominator;
                                denominator = remainder;
                            }

                            result.fixed8[0] = result.fixed8[0] / remainder;
                            result.fixed8[1] = result.fixed8[1] / remainder;
                        }

                        //If the denominator is 1, then it's no longer a fraction.
                        if(result.fixed8[1] == 1){
                            result.type = NumberType::fixed;
                            result.fixed8[1] = 0;
                        }

                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.float8[0] = ((double) left.fixed8[0]) / right.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = ((double) left.fixed8[0]) * right.float8[0] / (right.float8[0] * right.float8[0] + right.float8[1] * right.float8[1]);
                        result.float8[1] = -((double) left.fixed8[0]) * right.float8[1] / (right.float8[0] * right.float8[0] + right.float8[1] * right.float8[1]);
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::fraction:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::fraction;
                        result.fixed8[0] = left.fixed8[0];
                        result.fixed8[1] = left.fixed8[1] * right.fixed8[1];

                        //Determine the lowest common denominator of the fraction, if one exists.
                        if(result.fixed8[0] % result.fixed8[1] == 0){
                            int64_t numerator = result.fixed8[0];
                            int64_t denominator = result.fixed8[1];
                            int64_t remainder = 0;

                            while(remainder != 0){
                                remainder = denominator % remainder;

                                numerator = denominator;
                                denominator = remainder;
                            }

                            result.fixed8[0] = result.fixed8[0] / remainder;
                            result.fixed8[1] = result.fixed8[1] / remainder;
                        }

                        //If the denominator is 1, then it's no longer a fraction.
                        if(result.fixed8[1] == 1){
                            result.type = NumberType::fixed;
                            result.fixed8[1] = 0;
                        }

                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::fraction;
                        result.fixed8[0] = left.fixed8[0] * right.fixed8[1];
                        result.fixed8[1] = left.fixed8[1] * right.fixed8[0];

                        //Determine the lowest common denominator of the fraction, if one exists.
                        if(result.fixed8[0] % result.fixed8[1] == 0){
                            int64_t numerator = result.fixed8[0];
                            int64_t denominator = result.fixed8[1];
                            int64_t remainder = 0;

                            while(remainder != 0){
                                remainder = denominator % remainder;

                                numerator = denominator;
                                denominator = remainder;
                            }

                            result.fixed8[0] = result.fixed8[0] / remainder;
                            result.fixed8[1] = result.fixed8[1] / remainder;
                        }

                        //If the denominator is 1, then it's no longer a fraction.
                        if(result.fixed8[1] == 1){
                            result.type = NumberType::fixed;
                            result.fixed8[1] = 0;
                        }

                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.fixed8[0] = ((double) left.fixed8[1]) / ((double) left.fixed8[0]) * right.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = ((double) left.fixed8[1]) / ((double) left.fixed8[0]) * right.float8[0];
                        result.float8[1] = ((double) left.fixed8[1]) / ((double) left.fixed8[0]) * right.float8[0];
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::floating:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::floating;
                        result.float8[0] = left.float8[0] / ((double) right.fixed8[0]);
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::floating;
                        result.float8[0] = left.float8[0] * ((double) right.fixed8[1]) / ((double) right.fixed8[0]);
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.float8[0] = left.float8[0] / right.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] * right.float8[0] / (right.float8[0] * right.float8[0] + right.float8[1] * right.float8[1]);
                        result.float8[1] = -left.float8[1] * right.float8[1] / (right.float8[0] * right.float8[0] + right.float8[1] * right.float8[1]);
                        return result;
                    default:
                        //Unreachable.
                        return result;
                }
            case NumberType::complex:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] / ((double) right.fixed8[0]);
                        result.float8[1] = left.float8[1] / ((double) right.fixed8[0]);
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] * ((double) right.fixed8[1]) / ((double) right.fixed8[0]);
                        result.float8[1] = left.float8[1] * ((double) right.fixed8[1]) / ((double) right.fixed8[0]);
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::complex;
                        result.float8[0] = left.float8[0] / right.float8[0];
                        result.float8[1] = left.float8[1] / right.float8[0];
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = (left.float8[0] * right.float8[0] + left.float8[1] * right.float8[1]) / (right.float8[0] * right.float8[0] + right.float8[1] * right.float8[1]);
                        result.float8[1] = (right.float8[0] * left.float8[1] - right.float8[1] * left.float8[0]) / (right.float8[0] * right.float8[0] + right.float8[1] * right.float8[1]);
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
        double logMagnitude = 0.5 * log(float8[0] * float8[0] + float8[1] * float8[1]);
        double angle = atan2(float8[1], float8[0]);
        //For the cases when the right is indeed a fixed8.
        //Otherwise it's ignorable trash.
        int64_t remainingExponent = right.fixed8[0];
        int64_t bitmask = 1;
        bool isNegative;
        Number result;
        //A handy variable for generating the result from the base.
        Number handy;
        result.fixed8[0] = 1;

        switch(type){
            case NumberType::fixed:
                switch(right.type){
                    //The algorithm here is me abusing exponential growth.
                    //Each bit in the exponnet tells me which exponentials of the base to use.
                    case NumberType::fixed:
                        result.type = NumberType::fixed;
                        isNegative = right.fixed8[0] < 0;

                        if(isNegative){
                            right.fixed8[0] = -right.fixed8[0];
                        }

                        while(remainingExponent != 0){
                            //If the bit is 0, then I don't want to multiply the result.
                            if(bitmask & remainingExponent == 1){
                                result = result * handy;
                            }
                            handy = handy * handy;
                            remainingExponent = remainingExponent >> 1;
                        }

                        //If the result is negative, then it is to be reciprocated.
                        if(isNegative){
                            result.type = NumberType::fraction;
                            result.fixed8[1] = result.fixed8[0];
                            result.fixed8[0] = 1;
                        }
                        return result;
                    case NumberType::fraction:
                        //A fractional exponent will always yield a floating point value.
                        //Leaving it as a fractional exponent will just complicate the math a lot for me.
                        result.type = NumberType::floating;
                        result.float8[0] = pow((double) this->fixed8[0], ((double) right.fixed8[0]) / ((double) right.fixed8[1]));
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.float8[0] = pow((double) this->fixed8[0], right.float8[0]);
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = exp(log((double) fixed8[0]) * right.float8[0]) * cos(((double) fixed8[0]) * right.float8[1]);
                        result.float8[1] = exp(log((double) fixed8[0]) * right.float8[0]) * sin(((double) fixed8[0]) * right.float8[1]);
                        return result;
                    default:
                        //Unreachable
                        return result;
                }
            case NumberType::fraction:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::fraction;
                        isNegative = right.fixed8[0] < 0;

                        if(isNegative){
                            right.fixed8[0] = -right.fixed8[0];
                        }

                        while(remainingExponent != 0){
                            //If the bit is 0, then I don't want to multiply the result.
                            if(bitmask & remainingExponent == 1){
                                result = result * handy;
                            }

                            handy = handy * handy;
                            remainingExponent = remainingExponent >> 1;
                        }

                        if(isNegative){
                            handy.fixed8[0] = result.fixed8[0];
                            result.fixed8[0] = result.fixed8[1];
                            result.fixed8[1] = handy.fixed8[0];
                        }

                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::floating;
                        result.float8[0] = pow(((double) fixed8[0]) / ((double) fixed8[1]), ((double) right.fixed8[0]) / ((double) right.fixed8[1]));
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        result.float8[0] = pow(((double) fixed8[0]) / ((double) fixed8[1]), right.float8[0]);
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = exp(log(((double) fixed8[0]) / ((double) fixed8[1])) * right.float8[0]) * cos(log(((double) fixed8[0]) / ((double) fixed8[1])) * right.float8[1]);
                        result.float8[0] = exp(log(((double) fixed8[0]) / ((double) fixed8[1])) * right.float8[0]) * sin(log(((double) fixed8[0]) / ((double) fixed8[1])) * right.float8[1]);
                        return result;
                    default:
                        //Unreachable
                        return result;
                }
            case NumberType::floating:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::floating;
                        result.float8[0] = pow(float8[0], (double) right.fixed8[0]);
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::floating;
                        result.float8[0] = pow(float8[0], ((double) right.fixed8[0]) / ((double) right.fixed8[1]));
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::floating;
                        //Edge case: The base is negative.
                        if(float8[0] < 0){
                            result.type = NumberType::floating;
                            result.float8[0] = pow(float8[0], right.float8[0]) * cos(3.14159265358979323846 * right.float8[0]);
                            result.float8[1] = pow(float8[0], right.float8[0]) * sin(3.14159265358979323846 * right.float8[0]);
                            return result;
                        }
                        result.float8[0] = pow(float8[0], right.float8[0]);
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = exp(log(float8[0]) * right.fixed8[0]) * cos(log(float8[0]) * right.fixed8[1]);
                        result.float8[1] = exp(log(float8[0]) * right.fixed8[0]) * sin(log(float8[0]) * right.fixed8[1]);
                        return result;
                    default:
                        //Unreachable
                        return result;
                }
            case NumberType::complex:
                switch(right.type){
                    case NumberType::fixed:
                        result.type = NumberType::complex;
                        result.float8[0] = exp(right.fixed8[0] * logMagnitude) * cos(right.fixed8[0] * angle);
                        result.float8[1] = exp(right.fixed8[0] * logMagnitude) * sin(right.float8[0] * angle);
                        return result;
                    case NumberType::fraction:
                        result.type = NumberType::complex;
                        result.float8[0] = exp(((double) right.fixed8[0]) / ((double) right.fixed8[1]) * logMagnitude) * cos(angle * ((double) right.fixed8[0]) / ((double) right.fixed8[1]));
                        result.float8[1] = exp(((double) right.fixed8[0]) / ((double) right.fixed8[1]) * logMagnitude) * sin(angle * ((double) right.fixed8[0]) / ((double) right.fixed8[1]));
                        return result;
                    case NumberType::floating:
                        result.type = NumberType::complex;
                        result.float8[0] = exp(right.float8[0] * logMagnitude) * cos(angle * right.float8[0]);
                        result.float8[1] = exp(right.float8[0] * logMagnitude) * sin(angle * right.float8[0]);
                        return result;
                    case NumberType::complex:
                        result.type = NumberType::complex;
                        result.float8[0] = exp(logMagnitude * right.float8[0] - angle * right.float8[1]) * cos(logMagnitude * right.float8[1] + angle * right.float8[0]);
                        result.float8[1] = exp(logMagnitude * right.float8[0] - angle * right.float8[1]) * sin(logMagnitude * right.float8[1] + angle * right.float8[0]);
                        return result;
                    default:
                        //Unreachable
                        return result;
                }
        }

        return result;
    }
};
#endif