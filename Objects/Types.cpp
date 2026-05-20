#include <cstdint>

enum NumberType : uint8_t{
    NULLTYPE,
    fixed,
    fraction,
    floating,
    complex,
    constant
};

/// @brief For using identifying types by the lexer.
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
    double float8[2];
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

};