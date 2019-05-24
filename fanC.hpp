#ifndef _FANC_H_
#define _FANC_H_

#include <string>
#include <vector>
using namespace std;
vector<Scope*> table;
vector<int> offsets;

class Node{};

class Expression : public Node{
public:
    Type type;
};

class ReturnType : public Node{};
class Void : public ReturnType{};

class Type : public ReturnType{ //  TODO: Abstract class
};

class IntType : public Type{};

class ByteType : public Type{};

class BooleanType : public Type{};

class BinaryExpression : public Expression{
public:
    Expression leftExp;
    Expression rightExp;
    Operation op;
};

class UnaryExpression : public Expression{};

class Operation : public Node{};

class BinaryOperation : public Operation{
public:
    char op;
};

class Multiplactive : public BinaryOperation{};

class Additive : public BinaryOperation{};
class Relop : public Operation{};

class BooleanOperation : public Operation{};

class Boolean : public UnaryExpression{};

class ID : public UnaryExpression{
public:
    string name;
    int offset;
};

class Call : public UnaryExpression{
public:
    ID id;
    ReturnType returnType;
    vector< Expression > expressions;
};

class String : public UnaryExpression{
public:
    string value;
};

class Number : public UnaryExpression{};

class FormalDec{
public:
    Type type;
    ID id;
};

class FormalList : public Node{
public:
    vector<FormalDec> declerations;
};

class PreCondition : public Node{
public:
    Expression exp;
};

class PreConditions : public Node{
public:
    vector<PreCondition> preconditions;
};

class FuncDec : public Node{
    ReturnType returnType;
    ID id;
    FormalList arguments;
    PreConditions conditions;
};

class Scope{
public:
    vector<ID> ids;
    virtual void endScope(){}

};

class IfScope: public Scope{
public:
    void endScope() override{}

};

class WhileScope: public Scope{
public:
    void endScope() override{}

};

class FunctionScope: public Scope{
public:
    void endScope() override{}

};

#endif //FANC_H_