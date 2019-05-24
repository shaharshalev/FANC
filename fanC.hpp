#ifndef _FANC_H_
#define _FANC_H_

#include <string>
#include <vector>

class Node{};

class Expression : Node{
public:
    Type type;
};

class ReturnType : Node{};
class Void : ReturnType{};

class Type : ReturnType{ //  TODO: Abstract class
};

class IntType : Type{};

class ByteType : Type{};

class BooleanType : Type{};

class BinaryExpression : Expression{
public:
    Expression leftExp;
    Expression rightExp;
    Operation op;
};

class UnaryExpression : Expression{};

class Operation : Node{};

class BinaryOperation : Operation{
public:
    char op;
};

class Multiplactive : BinaryOperation{};

class Additive : BinaryOperation{};
class Relop : Operation{};

class BooleanOperation : Operation{};

class Boolean : UnaryExpression{};

class ID : UnaryExpression{
public:
    string name;
};

class Call : UnaryExpression{
public:
    ID id;
    ReturnType returnType;
    vector<Expression> expressions;
};

class String : UnaryExpression{
public:
    string value;
};

class Number : UnaryExpression{};

class FormalDec{
public:
    Type type;
    ID id;
};

class FormalList : Node{
public:
    vector<FormalDec> declerations;
};

class PreCondition : Node{
public:
    Expression exp;
};

class PreConditions : Node{
public:
    vector<PreCondition> preconditions;
};

class FuncDec : Node{
    ReturnType returnType;
    ID id;
    FormalList arguments;
    PreConditions conditions;
};

#endif //FANC_H_