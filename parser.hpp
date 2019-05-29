#ifndef _PARSER_H
#define _PARSER_H

#include <string>
#include <iostream>
#include <vector>
#include <typeinfo>
#include <sstream>
#include <stdbool.h>
#include <stdlib.h>     /* atoi */
#include "output.hpp"

using namespace std;
using namespace output;
extern int yylineno;
extern int yyparse (void);

#define YYSTYPE FanC::Node*
namespace FanC{

    class Relop;
    class Id;
    class BooleanOperation;
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

    enum IdentifierType{
        VariableType, FunctionType
    };

    class Node {
    public:
        virtual ~Node() {}
    };

    class ReturnType : public Node {
    public:
        virtual string typeName()=0;
        virtual ReturnType* clone()=0;
        virtual ~ReturnType() {}

    };

    class Type : public ReturnType {
    public:
        virtual string typeName(){return "Error<typeName method was called from Type class>";}
        Type* clone(){
            return new Type();
        }
        virtual ~Type() {}
    };

    class StringType : public Type{
            public:
            virtual string typeName(){return "STRING";}
            StringType* clone(){
                return new StringType();;
            }
            virtual ~StringType() {}
    };



    class Void : public ReturnType {
    public:
        virtual string typeName(){return "VOID";}
        Void* clone(){
            return new Void();
        }
        virtual ~Void() {
        }
    };

    class IntType : public Type {
    public:
        virtual string typeName(){return "INT";}
        IntType* clone(){
            return new IntType();
        }
        virtual ~IntType() {}
    };

    class ByteType : public Type {
    public:
        virtual string typeName(){return "BYTE";}
        ByteType* clone(){
            return new ByteType();
        }
        virtual ~ByteType() {}
    };

    class BooleanType : public Type {
    public:
        virtual string typeName(){return "BOOL";}
        BooleanType* clone(){
            return new BooleanType();
        }
        virtual ~BooleanType() {}
    };

    class Expression : public Node {
    public:
        ReturnType *type;


        Expression(ReturnType *_type) : type(_type) {}

        virtual Id* isPreconditionable()=0;

        virtual ~Expression() {
            delete type;
        }
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

        Id* isPreconditionable(){
            Id *id =leftExp->isPreconditionable();
            if( id!= NULL) return id;
            id = rightExp->isPreconditionable();
            return id;
        }

        BinaryExpression(Expression *_leftExp, Expression *_rightExp, Operation *_op)
                : Expression(NULL), leftExp(_leftExp), rightExp(_rightExp), op(_op) {
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

        Id* isPreconditionable(){
            return NULL;
        }

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

        Id* isPreconditionable(){
            return NULL;
        }

        virtual ~Boolean() {}
    };

    class Id : public UnaryExpression {
    public:
        string name;
        int offset;
        IdentifierType idType;
        friend bool operator==(const Id& ,const Id& );
        friend bool operator!=(const Id& ,const Id& );

        Id(string text) : UnaryExpression(new Type()), name(text),idType(VariableType) {}

        Id(string text,ReturnType* _type,IdentifierType _idType) : UnaryExpression(_type),
        name(text),idType(_idType) {}

        Id(Id* id):UnaryExpression(id->type) {
                name = id->name;
                offset = id->offset;
                idType = id->idType;
        }

        Id* isPreconditionable(){
            if(offset<0)return NULL;
            return this;
        }

        Id* changeIdTypeToFunction(){
            idType=FunctionType;
            return this;
        }
        Id* updateType(Type* t){
            delete this->type;
            this->type = t;
            return this;
        }
        bool isFunction(){
            return idType==FunctionType;
        }

        virtual ~Id() {}
    };


    class String : public UnaryExpression {
    public:
        string value;

        String(string text) : UnaryExpression(new StringType()), value(text) {}

        Id* isPreconditionable(){
            return NULL;
        }

        virtual ~String() {}
    };

    class Number : public UnaryExpression {
    public:
        int value;

        Number(string text, Type *_type) : UnaryExpression(_type), value(atoi(text.c_str())) {}

        Number(int val, Type *_type) : UnaryExpression(_type), value(val) {}

        Id* isPreconditionable(){
            return NULL;
        }

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
        Type* type;
        Id* id;

        FormalDec(Type *_type, Id *_id) : type(_type), id(_id) {
        }

        virtual ~FormalDec() {
            delete type;
        }


    };

    class FormalList : public Node {
    public:
        vector<FormalDec *> decelerations;

        FormalList() {}

        FormalList(FormalDec *formalDec) {
            add(formalDec);
        }

        FormalList* add(FormalDec *formalDec) {
                decelerations.insert(decelerations.begin(), formalDec);
            return this;
        }

        int size(){return decelerations.size(); }

        virtual ~FormalList() {}
    };

    class PreCondition : public Node {
    public:
        Expression *exp;

        PreCondition(Expression *_exp) : exp(_exp) {}

        Id* isExpressionValid(){
            return exp->isPreconditionable();
        }

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

        Id* isValid(){
            vector<PreCondition *>::iterator it=preconditions.begin();
            while(it!=preconditions.end()){
                Id *i = (*it)->isExpressionValid();
                if(i!=NULL)
                    return i;
                ++it;
            }
            return NULL;
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
        friend bool operator!=(const FuncDec& ,const FuncDec& );


        FuncDec(ReturnType *_returnType,
                Id *_id, FormalList *_arguments,
                PreConditions *_conditions) : returnType(_returnType),
                                              id(_id),
                                              arguments(_arguments),
                                              conditions(_conditions) {}



        vector<string>* getArgsAsString(){
            vector<FormalDec *>::iterator it = this->arguments->decelerations.begin();
            vector<string>* typesName = new vector<string>();
            while(it != arguments->decelerations.end()){
                typesName->push_back((*it)->type->typeName());
                ++it;
            }
            return typesName;
        }


        bool isArgumentListMatch(ExpressionList* expList){
            vector<Expression *>::iterator expIt = expList->expressions.begin();
            vector<FormalDec *>::iterator formalIt = this->arguments->decelerations.begin();
            while( (expIt != expList->expressions.end()) && (formalIt != arguments->decelerations.end())){
                Expression* currentExp=*expIt;
                FormalDec* currentArgument=*formalIt;
                if(currentExp->type->typeName() != currentArgument->type->typeName()) return false;
                ++expIt;
                ++formalIt;
            }
            if(expIt != expList->expressions.end() || formalIt != arguments->decelerations.end())
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
        ExpressionList *expressions;

        Call(ReturnType *_returnType, Id *_id, ExpressionList *_expressions)
                : UnaryExpression(_returnType),id(_id), expressions(_expressions) {}

        Id* isPreconditionable(){
            vector<Expression*>::iterator it=expressions->expressions.begin();
            while(it!=expressions->expressions.end()){
                Id* i=(*it)->isPreconditionable();
                if(NULL!=i)
                    return i;
                ++it;
            }
            return NULL;
        }

        virtual ~Call() {
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
            func->id->changeIdTypeToFunction();
            functions.push_back(func);
            Id * i = new Id(func->id);
            variables.push_back(i);

        }

    private:
        bool isFunctionExistInScope(FuncDec* func){
            vector<FuncDec*>::iterator it=functions.begin();
            while(it!=functions.end()){
                FuncDec* currentFunc=*it;
                if(*func == *currentFunc)
                    return true;
                ++it;
            }
            return false;
        }


        FuncDec* getFunctionInScope(Id* id){

            for (vector<FuncDec *>::iterator it = functions.begin(); it != functions.end(); ++it) {
                FuncDec* currentFunc=*it;
                if( *currentFunc->id == *id){
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

            vector<Id *>::iterator it=variables.begin();
            while( it!= variables.end()){
                Id* currentId= *it;
                if(*id==*currentId)
                    return *it;
                ++it;
            }
            return NULL;
        }

    public:
        /**
         * the method gets a variable/function Id from this scope or parents scopes.
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

        void printIds(){
            for (vector<Id *>::iterator it = variables.begin(); it != variables.end(); ++it) {
                if((*it)->isFunction()){
                    FuncDec* fun = getFunction(*it);
                    vector<string>* args=fun->getArgsAsString();
                    string func_type=makeFunctionType(fun->returnType->typeName(), *args);
                    delete args;
                    output::printID((*it)->name,0,func_type);
                }else {
                    output::printID((*it)->name, (*it)->offset, (*it)->type->typeName());
                }
            }
        }


        virtual void endScope() {
            output::endScope();
            printIds();

        }

        virtual ~Scope(){
            for (vector<Id *>::iterator it = variables.begin(); it != variables.end(); ++it) {
                delete *it;
            }
            for (vector<FuncDec *>::iterator it = functions.begin(); it != functions.end(); ++it) {
                delete *it;
            }

        }

    };//End of class Scope


    class WhileScope : public Scope {
    public:
        WhileScope(Scope* _parent):Scope(_parent){}
        virtual ~WhileScope(){

        }
    };

    class FunctionScope : public Scope {
    public:
        FuncDec* func;

        FunctionScope(Scope* _parent):Scope(_parent){}

        void updateFunctionScope(FuncDec* _func){
            func=_func;
            if(NULL != this->parent)
                parent->addFunction(_func);
        }
        void updateFunctionScope(PreConditions* preconditions){
            func->conditions=preconditions;
        }

        FuncDec* getFunction(){
            return func;
        }

        void endScope()  {
            output::endScope();
            output::printPreconditions(func->id->name,func->conditions->size());
            printIds();
        }
        virtual ~FunctionScope(){

        }
    };
    
}

#endif //_PARSER_H