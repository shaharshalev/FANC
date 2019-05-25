#ifndef _PARSER_H
#define _PARSER_H
#include <string>
#include <iostream>
#include <vector>
#include "output.hpp"
#include <typeinfo>
#include <sstream>
#include <stdbool.h>
#include <stdlib.h>     /* atoi */
#define YYSTYPE Node*
using namespace std;
using namespace output;
extern int yylineno;

enum BoolOp{And,Or};
class Relop;
class BooleanOperation;


class Node{
public:
    virtual ~Node(){}
};

class ReturnType : public Node{
public:
    virtual ~ReturnType(){}
};

class Type : public ReturnType{
public:
    virtual ~Type(){}
};

class Expression : public Node{
public:
    ReturnType* type;
    Expression(ReturnType* _type):type(_type){}

    virtual ~Expression(){
        delete type;
    }
};

class Void : public ReturnType{
public:
    virtual ~Void(){
    }
};

class IntType : public Type{
public:
    virtual ~IntType(){}
};

class ByteType : public Type{
public:
    virtual ~ByteType(){}
};

class BooleanType : public Type{
public:
    virtual ~BooleanType(){}
};

class Operation : public Node{
public:
    virtual ~Operation(){}
};

template<typename CheckType,typename InstanceType>
bool isInstanceOf(InstanceType* instance){
    return (dynamic_cast<CheckType*>(instance) != NULL);
}

template<typename CheckType,typename InstanceType1,typename InstanceType2>
bool isSameType(InstanceType1* instance1,InstanceType2* instance2){
    return (dynamic_cast<CheckType*>(instance1)!=NULL && dynamic_cast<CheckType*>(instance2)!=NULL);
}

class BinaryExpression : public Expression{
public:
    Expression* leftExp;
    Expression* rightExp;
    Operation* op;
    BinaryExpression(Expression* _leftExp, Expression* _rightExp,Operation* _op)
    :Expression(new Type()),leftExp(_leftExp),rightExp(_rightExp),op(_op) {
        if ( isInstanceOf<Relop> (_op) || isInstanceOf<BooleanOperation>(_op)) {
            this->type = new BooleanType();
        } else {
            //if they are not of the same type we should take the larger one
            //(will always be int in our case)
            if(isSameType<ByteType>(leftExp->type,rightExp->type)
                    || isSameType<IntType>(leftExp->type,rightExp->type) ){
                this->type = leftExp->type;
            }else{
                this->type = new IntType();
            }
        }
    }
    virtual ~BinaryExpression(){
        delete leftExp;
        delete rightExp;
        delete op;
    }
};

class UnaryExpression : public Expression{
public:
    UnaryExpression(ReturnType* _type):Expression(_type){}

    virtual ~UnaryExpression(){
    };
};

class Not : public UnaryExpression{
public:
    Expression *exp;
    Not(Expression* _exp):UnaryExpression(new BooleanType()),exp(_exp){}

    virtual ~Not(){
       delete(exp);
    };
};


class BinaryOperation : public Operation{
public:
    string op;

    BinaryOperation(string text):op(text){}

    virtual ~BinaryOperation(){}
};

class Multiplicative : public BinaryOperation{
    public:
    Multiplicative(string text):BinaryOperation(text){}

    virtual ~Multiplicative(){}
};

class Additive : public BinaryOperation{
public:
    Additive(string text):BinaryOperation(text){}

    virtual ~Additive(){}
};

class Relop : public Operation{
public:
    string op;

    Relop(string text):op(text){}

    virtual ~Relop(){}
};

class BooleanOperation : public Operation{
public:
    BoolOp op;
    BooleanOperation(BoolOp o):op(o){}
    virtual ~BooleanOperation(){}
};

class Boolean : public UnaryExpression{
public:
    bool value;
    Boolean(bool val):UnaryExpression(new BooleanType()),value(val){}
    virtual ~Boolean(){}
};

class Id : public UnaryExpression{
public:
    string name;
    int offset;

    Id(string text ):UnaryExpression(new Type()),name(text){}
    virtual ~Id(){}
};

class Call : public UnaryExpression{
public:
    Id id;
    ReturnType* returnType;
    vector< Expression* > expressions;
    Call(ReturnType* _returnType,Id _id,vector< Expression* > _expressions)
    :UnaryExpression(_returnType),returnType(_returnType),id(_id),expressions(_expressions){}

    virtual ~Call(){
        for(vector<Expression*>::iterator it = expressions.begin(); it!= expressions.end();++it){
            delete *it;
        }
        delete returnType;
    }
};

class String : public UnaryExpression{
public:
    string value;
    String(string text):UnaryExpression(new Type()),value(text){}

    virtual ~String(){}
};

class Number : public UnaryExpression{
public:
    int value;
    Number(string text,Type* _type):UnaryExpression(_type),value(atoi(text.c_str())){}
    Number(int val,Type* _type):UnaryExpression(_type),value(val){}

    virtual ~Number(){}

};

class Integer : public Number{
public:
    Integer(Number* n):Number(n->value,new IntType()){
        delete n;
    }

    virtual  ~Integer(){}
};

class Byte : public Number{
public:
    Byte(Number* num):Number(num->value,new ByteType()){
        delete num; //check
        if(value > 255){
            stringstream stream;
            stream<<value;
            errorByteTooLarge(yylineno, stream.str());
            exit(1);
        }
    }

    virtual  ~Byte(){}
};

class FormalDec : public Node{
public:
    Type* type;
    Id id;

    virtual ~FormalDec(){
        delete type;
    }


};

class FormalList : public Node {
public:
    vector<FormalDec> declerations;

    virtual ~FormalList(){}
};

class PreCondition : public Node {
public:
    Expression* exp;

    PreCondition(Expression* _exp):exp(_exp){}

    virtual ~PreCondition(){
        delete exp;
    }
};

class PreConditions : public Node {
public:
    vector<PreCondition> preconditions;

    virtual ~PreConditions(){}
};

class FuncDec : public Node {
    ReturnType* returnType;
    Id id;
    FormalList arguments;
    PreConditions conditions;

    virtual ~FuncDec(){
        delete returnType;
    }
};

class Scope{
public:
    vector<Id> ids;
    virtual void endScope(){}


};

class IfScope: public Scope {
public:
    void endScope(){}

};

class WhileScope: public Scope {
public:
    void endScope(){}

};

class FunctionScope: public Scope {
public:
    void endScope(){}
};



#endif //_PARSER_H