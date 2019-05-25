#ifndef _FANC_H_
#define _FANC_H_
#include <string>
#include <iostream>
#include <vector>
#include "output.hpp"
#include <sstream>
#define YYSTYPE Node*
using namespace std;
using namespace output;
extern int yylineno;


class Node{};

class ReturnType : public Node{};

class Type : public ReturnType{ 
};

class Expression : public Node{
public:
    Type type;
};

class Void : public ReturnType{};

class IntType : public Type{};

class ByteType : public Type{};

class BooleanType : public Type{};

class Operation : public Node{
    
};

class BinaryExpression : public Expression{
public:
    Expression leftExp;
    Expression rightExp;
    Operation op;
};

class UnaryExpression : public Expression{};


class BinaryOperation : public Operation{
public:
    char op;

    BinaryOperation(string text):op(text[0]){}
};

class Multiplicative : public BinaryOperation{
    public:
    Multiplicative(string text):BinaryOperation(text){}
};

class Additive : public BinaryOperation{
public:
    Additive(string text):BinaryOperation(text){}
};

class Relop : public Operation{
public:
    string op;

    Relop(string text):op(text){}
};

class BooleanOperation : public Operation{};

class Boolean : public UnaryExpression{};

class Id : public UnaryExpression{
public:
    string name;
    int offset;

    Id(string text ):name(text){}
};

class Call : public UnaryExpression{
public:
    Id id;
    ReturnType returnType;
    vector< Expression > expressions;
};

class String : public UnaryExpression{
public:
    string value;

    String(string text):value(text){}
};

class Number : public UnaryExpression{
public:
    int value;
    Number(string text):value(atoi(text.c_str())){}
    Number(int val):value(val){}

};

class Integer : public Number{
public:
    Integer(Number* n):Number(n->value){
        delete n;
    }
};

class Byte : public Number{
public:
    Byte(Number* num):Number(num->value){
        delete num; //check
        if(value > 255){
            stringstream stream;
            stream<<value;
            errorByteTooLarge(yylineno, stream.str());
            exit(1);
        }
    }
};

class FormalDec{
public:
    Type type;
    Id id;
};

class FormalList : public Node {
public:
    vector<FormalDec> declerations;
};

class PreCondition : public Node {
public:
    Expression exp;
};

class PreConditions : public Node {
public:
    vector<PreCondition> preconditions;
};

class FuncDec : public Node {
    ReturnType returnType;
    Id id;
    FormalList arguments;
    PreConditions conditions;
};

class Scope{
public:
    vector<Id> ids;
    virtual void endScope();

};

class IfScope: public Scope {
public:
    void endScope();

};

class WhileScope: public Scope {
public:
    void endScope();

};

class FunctionScope: public Scope {
public:
    void endScope();

};




#endif //FANC_H_