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
#include <algorithm>    // std::find

#define YYSTYPE Node*
using namespace std;
using namespace output;
extern int yylineno;

template<typename CheckType, typename InstanceType>
bool isInstanceOf(InstanceType *instance) {
    return (dynamic_cast<CheckType *>(instance) != NULL);
}

template<typename CheckType, typename InstanceType1, typename InstanceType2>
bool isSameType(InstanceType1 *instance1, InstanceType2 *instance2) {
    return (dynamic_cast<CheckType *>(instance1) != NULL && dynamic_cast<CheckType *>(instance2) != NULL);
}

enum BoolOp {
    And, Or
};
enum WhileOp {
    Break, Continue
};

class Relop;

class BooleanOperation;


class Node {
public:
    virtual ~Node() {}
};

class ReturnType : public Node {
public:
    virtual ~ReturnType() {}
};

class Type : public ReturnType {
public:
    virtual ~Type() {}
};


class Expression : public Node {
public:
    ReturnType *type;

    Expression(ReturnType *_type) : type(_type) {}

    virtual ~Expression() {
        delete type;
    }
};

class Void : public ReturnType {
public:

    virtual ~Void() {
    }
};

class IntType : public Type {
public:
    virtual ~IntType() {}
};

class ByteType : public Type {
public:
    virtual ~ByteType() {}
};

class BooleanType : public Type {
public:
    virtual ~BooleanType() {}
};

class Operation : public Node {
public:
    virtual ~Operation() {}
};



class BinaryExpression : public Expression {
public:
    Expression *leftExp;
    Expression *rightExp;
    Operation *op;

    BinaryExpression(Expression *_leftExp, Expression *_rightExp, Operation *_op)
            : Expression(new Type()), leftExp(_leftExp), rightExp(_rightExp), op(_op) {
        if (isInstanceOf<Relop>(_op) || isInstanceOf<BooleanOperation>(_op)) {
            this->type = new BooleanType();
        } else {
            //if they are not of the same type we should take the larger one
            //(will always be int in our case)
            if (isSameType<ByteType>(leftExp->type, rightExp->type)
                || isSameType<IntType>(leftExp->type, rightExp->type)) {
                this->type = leftExp->type;
            } else {
                this->type = new IntType();
            }
        }
    }

    virtual ~BinaryExpression() {
        delete leftExp;
        delete rightExp;
        delete op;
    }
};

class UnaryExpression : public Expression {
public:
    UnaryExpression(ReturnType *_type) : Expression(_type) {}

    virtual ~UnaryExpression() {
    };
};

class Not : public UnaryExpression {
public:
    Expression *exp;

    Not(Expression *_exp) : UnaryExpression(new BooleanType()), exp(_exp) {}

    virtual ~Not() {
        delete (exp);
    };
};


class BinaryOperation : public Operation {
public:
    string op;

    BinaryOperation(string text) : op(text) {}

    virtual ~BinaryOperation() {}
};

class Multiplicative : public BinaryOperation {
public:
    Multiplicative(string text) : BinaryOperation(text) {}

    virtual ~Multiplicative() {}
};

class Additive : public BinaryOperation {
public:
    Additive(string text) : BinaryOperation(text) {}

    virtual ~Additive() {}
};

class Relop : public Operation {
public:
    string op;

    Relop(string text) : op(text) {}

    virtual ~Relop() {}
};

class BooleanOperation : public Operation {
public:
    BoolOp op;

    BooleanOperation(BoolOp o) : op(o) {}

    virtual ~BooleanOperation() {}
};

class Boolean : public UnaryExpression {
public:
    bool value;

    Boolean(bool val) : UnaryExpression(new BooleanType()), value(val) {}

    virtual ~Boolean() {}
};

class Id : public UnaryExpression {
public:
    string name;
    int offset;
    friend bool operator==(const Id& ,const Id& );

    Id(string text) : UnaryExpression(new Type()), name(text) {}

    Id* updateType(Type* t){
        delete this->type;
        this->type = t;
        return this;
    }

    virtual ~Id() {}
};


class String : public UnaryExpression {
public:
    string value;

    String(string text) : UnaryExpression(new Type()), value(text) {}

    virtual ~String() {}
};

class Number : public UnaryExpression {
public:
    int value;

    Number(string text, Type *_type) : UnaryExpression(_type), value(atoi(text.c_str())) {}

    Number(int val, Type *_type) : UnaryExpression(_type), value(val) {}

    virtual ~Number() {}

};

class Integer : public Number {
public:
    Integer(Number *n) : Number(n->value, new IntType()) {
        delete n;
    }

    virtual  ~Integer() {}
};

class Byte : public Number {
public:
    Byte(Number *num) : Number(num->value, new ByteType()) {
        delete num; //check
        if (value > 255) {
            stringstream stream;
            stream << value;
            errorByteTooLarge(yylineno, stream.str());
            exit(1);
        }
    }

    virtual  ~Byte() {}
};

class FormalDec : public Node {
public:
    Type *type;
    Id *id;

    FormalDec(Type *_type, Id *_id) : type(_type), id(_id) {}

    virtual ~FormalDec() {
        delete type;
    }


};

class FormalList : public Node {
public:
    vector<FormalDec *> declerations;

    FormalList() {}

    FormalList(FormalDec *formalDec) {
        add(formalDec);
    }

    FormalList *add(FormalDec *formalDec) {
        declerations.insert(declerations.begin(), formalDec);
        return this;
    }

    virtual ~FormalList() {}
};

class PreCondition : public Node {
public:
    Expression *exp;

    PreCondition(Expression *_exp) : exp(_exp) {}

    virtual ~PreCondition() {
        delete exp;
    }
};

class PreConditions : public Node {
public:
    vector<PreCondition *> preconditions;

    PreConditions() {}

    PreConditions *add(PreCondition *precond) {
        preconditions.insert(preconditions.begin(), precond);
        return this;
    }

    int size() {
        return preconditions.size();
    }

    virtual ~PreConditions() {
        for (vector<PreCondition *>::iterator it = preconditions.begin(); it != preconditions.end(); ++it) {
            delete *it;
        }
    }
};



class ExpressionList : public Node {
        public:
        vector<Expression *> expressions;

        ExpressionList() {}

        ExpressionList(Expression *exp) {
            expressions.insert(expressions.begin(), exp);
        }

        ExpressionList *add(Expression *exp) {
            expressions.insert(expressions.begin(), exp);
            return this;
        }

        virtual ~ExpressionList() {
            for (vector<Expression *>::iterator it = expressions.begin(); it != expressions.end(); ++it) {
                delete *it;
            }
        }
};




class FuncDec : public Node {
public:
    ReturnType *returnType;
    Id *id;
    FormalList *arguments;
    PreConditions *conditions;
    friend bool operator==(const FuncDec& ,const FuncDec& );


    FuncDec(ReturnType *_returnType,
            Id *_id, FormalList *_arguments,
            PreConditions *_conditions) : returnType(_returnType),
                                          id(_id),
                                          arguments(_arguments),
                                          conditions(_conditions) {}


    vector<string>* getArgsAsString(){
        vector<FormalDec *>::iterator it = this->arguments->declerations.begin();
        vector<string>* typesName = new vector<string>();
        while(it != arguments->declerations.end()){
            typesName->push_back(typeid((*it)->type).name());
            ++it;
        }
        return typesName;
    }


    bool isArgumentListMatch(ExpressionList* expList){
        vector<Expression *>::iterator expIt = expList->expressions.begin();
        vector<FormalDec *>::iterator formalIt = this->arguments->declerations.begin();
        while( (expIt != expList->expressions.end()) && (formalIt != arguments->declerations.end())){
            if(typeid((*expIt)->type).name() != typeid((*formalIt)->type).name()) return false;
            ++expIt;
            ++formalIt;
        }
        if(expIt != expList->expressions.end() || formalIt != arguments->declerations.end())
            return false;

        return true;
    }


    virtual ~FuncDec() {
        delete returnType;
        delete id;
        delete arguments;
        delete conditions;
    }
};



class Call : public UnaryExpression {
public:
    Id *id;
    ReturnType *returnType;
    ExpressionList *expressions;

    Call(ReturnType *_returnType, Id *_id, ExpressionList *_expressions)
            : UnaryExpression(_returnType), returnType(_returnType), id(_id), expressions(_expressions) {}

    virtual ~Call() {
        delete returnType;
        delete expressions;
        delete id;
    }
};

class Scope {
public:
    vector<Id*> variables;
    vector<FuncDec*> functions;
    Scope* parent;
    Scope(){
        parent=NULL;
    }
    Scope(Scope* _parent):parent(_parent){}

    void addVariable(Id* id){
        variables.push_back(id);
    }
    void addFunction(FuncDec* func){
        functions.push_back(func);
    }
private:
    bool isFunctionExistInScope(FuncDec* func){
        return functions.end() != std::find(functions.begin(), functions.end(), func);
    }

    FuncDec* getFunctionInScope(Id* id){

        for (vector<FuncDec *>::iterator it = functions.begin(); it != functions.end(); ++it) {
            if( (*it)->id == id){
                return *it;
            }
        }
        return NULL;
    }

public:

    /**
     * the method check if function exist in this scope or above.
     * @param func is the function
     * @return true if exists, false otherwise
     */
    bool isFunctionExist(FuncDec* func){
        if(isFunctionExistInScope(func)) {
            return true;
        }else if(NULL != parent){
            return parent->isFunctionExist(func);
        }else{
            return false;
        }
    }
    virtual FuncDec* getFunction(Id* id){
        FuncDec* found=getFunctionInScope(id);
        if(NULL != found){
            return found;
        }else if(NULL != parent){
            return parent->getFunction(id);
        }else{
            return NULL;
        }
    }
private:
    Id* getVariableInScope(Id* id){
        vector<Id *>::iterator it=std::find(variables.begin(), variables.end(), id);
        if(it!= variables.end()){
            return *it;
        }else{
            return NULL;
        }
    }

public:
    /**
     * the method gets a variable from this scope or parents scopes.
     * @param id
     * @return the id from the symbolTable, NULL if no such id exist
     */
    Id* getVariable(Id* id){
        Id* found=getVariableInScope(id);
        if(NULL != found){
            return found;
        }else if(NULL != parent){
            return parent->getVariable(id);
        }else{
            return NULL;
        }
    }


    virtual void endScope() {}

    virtual ~Scope(){
        for (vector<Id *>::iterator it = variables.begin(); it != variables.end(); ++it) {
            delete *it;
        }
        for (vector<FuncDec *>::iterator it = functions.begin(); it != functions.end(); ++it) {
            delete *it;
        }

    }

};
/*
class IfScope : public Scope {
public:
    IfScope(Scope* _parent):Scope(_parent){}
    void endScope()  {}
    virtual ~IfScope(){

    }
};*/

class WhileScope : public Scope {
public:
    WhileScope(Scope* _parent):Scope(_parent){}
    void endScope()  {}
    virtual ~WhileScope(){

    }
};

class FunctionScope : public Scope {
public:
    FuncDec* func;
    FunctionScope(Scope* _parent,FuncDec* _func):Scope(_parent),func(_func){
        if(NULL != parent)
            parent->addFunction(_func);
    }
    FuncDec* getFunction(){
        return func;
    }

    void endScope()  {}
    virtual ~FunctionScope(){
    }
};


#endif //_PARSER_H