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
    Expression(Type _type):type(_type){}
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
    BinaryExpression(Expression _leftExp, Expression _rightExp,Operation _op)
    :Expression(Type()),leftExp(_leftExp),rightExp(_rightExp),op(_op){
    if( "Relop"==typeid(op).name() ||"BooleanOperation"==typeid(op).name())
    {
        this->type=BooleanType();
    }else{
        //if they are not of the same type we should take the larger one
        //(will always be int in our case)
        if(leftExp.type!=right.type){
            this->type=IntType();
        }else{
            this->type=leftExp.type;
        }
    }
};

class UnaryExpression : public Expression{
     UnaryExpression(Type _type):Expression(_type){}
};


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

class Boolean : public UnaryExpression{
    Boolean():UnaryExpression(BooleanType())
};

class Id : public UnaryExpression{
public:
    string name;
    int offset;

    Id(string text ):UnaryExpression(Type()),name(text){}
};

class Call : public UnaryExpression{
public:
    Id id;
    ReturnType returnType;
    vector< Expression > expressions;
    Call(ReturnType _returnType,Id _id,vector< Expression > _expressions)
    :UnaryExpression(_returnType),id(_id),_expressions)
};

class String : public UnaryExpression{
public:
    string value;
    String(string text):UnaryExpression(Type()),value(text){}
};

class Number : public UnaryExpression{
public:
    int value;
    Number(string text,Type _type):UnaryExpression(_type),value(atoi(text.c_str())){}
    Number(int val,Type _type)::UnaryExpression(_type),value(val){}

};

class Integer : public Number{
public:
    Integer(Number* n):Number(n->value,IntType()){
        delete n;
    }
};

class Byte : public Number{
public:
    Byte(Number* num):Number(num->value,ByteType()){
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