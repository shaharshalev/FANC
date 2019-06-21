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
#include "registers.hpp"
#include "bp.hpp"
#include "assembler_coder.hpp"
#include <assert.h>     /* assert */

using namespace std;
using namespace output;
extern int yylineno;

extern int yyparse(void);

#define YYSTYPE FanC::Node*
namespace FanC {

    class Relop;

    class Id;

    class BooleanOperation;

    class RelationalOperation;

    class EqualityOperation;

    class Multiplicative;

    class Additive;

    class Number;

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

    enum IdentifierType {
        VariableType, FunctionType
    };

    class Node {
    public:
        vector<int> trueList;
        vector<int> falseList;
        string registerName;
    protected:
        AssemblerCoder &assembler = AssemblerCoder::getInstance();
        Registers &registers = Registers::getInstance();
        CodeBuffer &codeBuffer = CodeBuffer::instance();
    public:
        Node() = default;

        virtual ~Node() = default;
    };

    //Label Marker- a label creator in some place
    class M : public Node {
    public:
        string label; // the "quad" - label or address in our case it is just a label
        M() : label("") {
            label = codeBuffer.genLabel();
        }

    };

    class N : public Node {
    public:
        vector<int> nextList;

        N() : nextList() {
            nextList = codeBuffer.makelist(assembler.j());//to be patched
        }
    };

    class ReturnType : public Node {
    public:
        virtual string typeName() = 0;

        virtual ReturnType *clone() = 0;

        virtual bool canBeAssigned(ReturnType *other) {
            return this->typeName() == other->typeName();
        }

        virtual ~ReturnType() = default;

    };

    class Type : public ReturnType {
    public:
        virtual string typeName() { return "Error<typeName method was called from Type class>"; }

        Type *clone() {
            return new Type();
        }

        virtual ~Type() = default;
    };

    class StringType : public Type {
    public:
        virtual string typeName() { return "STRING"; }

        StringType *clone() {
            return new StringType();
        }

        virtual ~StringType() = default;
    };


    class Void : public ReturnType {
    public:
        virtual string typeName() { return "VOID"; }

        Void *clone() {
            return new Void();
        }

        virtual ~Void() = default;
    };

    class ByteType : public Type {
    public:
        virtual string typeName() { return "BYTE"; }

        ByteType *clone() {
            return new ByteType();
        }

        virtual ~ByteType() = default;
    };

    class IntType : public Type {
    public:
        virtual string typeName() { return "INT"; }

        IntType *clone() {
            return new IntType();
        }

        bool canBeAssigned(ReturnType *other) {
            return this->typeName() == other->typeName() || other->typeName() == ByteType().typeName();
        }

        virtual ~IntType() = default;
    };


    class BooleanType : public Type {
    public:
        virtual string typeName() { return "BOOL"; }

        BooleanType *clone() {
            return new BooleanType();
        }

        virtual ~BooleanType() = default;
    };


    class Statement : public Node {
    public:
        vector<int> continueList;
        vector<int> breakList;

        Statement() : Node(), continueList(), breakList() {

        }

    };

    class Statements : public Node {
    public:
        vector<Statement *> statements;

        explicit Statements(Statement *statement) : statements() {
            add(statement);
        }

        Statements *add(Statement *statement) {
            statements.push_back(statement);
            return this;
        }

        virtual ~Statements() {
            for (vector<Statement *>::iterator it = statements.begin(); it != statements.end(); ++it) {
                delete *it;
            }
        }
    };


    class Expression : public Node {
    public:
        ReturnType *type;
        vector<int> trueList;
        vector<int> falseList;
        string registerName;

        explicit Expression(ReturnType *_type) : type(_type), trueList(), falseList(), registerName("") {}

        virtual Id *isPreconditionable() = 0;

        bool isBoolean() {
            return this->type->typeName() == BooleanType().typeName();
        }

        bool isNumric() {
            bool value = isInstanceOf<ByteType>(type) || isInstanceOf<IntType>(type);
            return value;
        }

        virtual ~Expression() {

            delete type;

        }
    };

    class Operation : public Node {
    public:
        virtual ~Operation() = default;
    };

    class BinaryOperation : public Operation {
    public:
        string op;

        explicit BinaryOperation(string text) : op(text) {}

        virtual ~BinaryOperation() = default;
    };

    class Relop : public Operation {
    public:
        string op;

        explicit Relop(string text) : op(text) {}

        virtual ~Relop() = default;
    };

    class BooleanOperation : public Operation {
    public:
        BoolOp op;

        explicit BooleanOperation(BoolOp o) : op(o) {}

        virtual ~BooleanOperation() {

        }
    };

    class BinaryExpression : public Expression {
    public:
        Expression *leftExp;
        Expression *rightExp;
        Operation *op;

        Id *isPreconditionable() {
            Id *id = leftExp->isPreconditionable();
            if (id != NULL) return id;
            id = rightExp->isPreconditionable();
            return id;
        }

        BinaryExpression(Expression *_leftExp, Expression *_rightExp, Operation *_op)
                : Expression(NULL), leftExp(_leftExp), rightExp(_rightExp), op(_op) {
            if (isInstanceOf<Relop>(_op)) {
                if (leftExp->isNumric() && rightExp->isNumric()) {
                    this->type = new BooleanType();
                    string _operation = ((Relop *) _op)->op;
                    int cmdAddress;
                    if (_operation == "==") {
                        cmdAddress = assembler.beq(leftExp->registerName, rightExp->registerName);
                    } else if (_operation == "!=") {
                        cmdAddress = assembler.bne(leftExp->registerName, rightExp->registerName);

                    } else if (_operation == "<=") {
                        cmdAddress = assembler.ble(leftExp->registerName, rightExp->registerName);
                    } else if (_operation == "<") {
                        cmdAddress = assembler.blt(leftExp->registerName, rightExp->registerName);
                    } else if (_operation == ">=") {
                        cmdAddress = assembler.bge(leftExp->registerName, rightExp->registerName);
                    } else { // _operation is >
                        cmdAddress = assembler.bgt(leftExp->registerName, rightExp->registerName);
                    }
                    this->trueList = codeBuffer.makelist(cmdAddress);
                    this->falseList = codeBuffer.makelist(assembler.j());
                    registers.regFree(leftExp->registerName);
                    registers.regFree(rightExp->registerName);
                } else {
                    errorMismatch(yylineno);
                    exit(1);
                }
            } else if (isInstanceOf<BooleanOperation>(_op)) {
                assert(false);// should go to the second ctor

            } else if (isInstanceOf<Multiplicative>(_op) || isInstanceOf<Additive>(_op)) {
                if (leftExp->isNumric() && rightExp->isNumric()) {
                    this->type = getLargerType();
                    this->registerName = leftExp->registerName;
                    string _operator = ((BinaryOperation *) _op)->op;
                    if (isInstanceOf<Multiplicative>(_op)) {
                        if (_operator == "*") {
                            assembler.mul(registerName, leftExp->registerName, rightExp->registerName);
                        } else if (_operator == "/") {
                            assembler.div(registerName, leftExp->registerName, rightExp->registerName);
                        }

                    } else { //Additive
                        if (_operator == "+")
                            assembler.addu(registerName, leftExp->registerName, rightExp->registerName);
                        else {
                            assembler.subu(registerName, leftExp->registerName, rightExp->registerName);
                        }

                    }
                } else {
                    errorMismatch(yylineno);
                    exit(1);
                }

                if (this->type->typeName() == ByteType().typeName()) {
                    assembler.andi(registerName, registerName, 255);
                }
                registers.regFree(rightExp->registerName);
            }


        }


        BinaryExpression(Expression *_leftExp, Expression *_rightExp, BooleanOperation *_op, M *beforeRhsMarker)
                : Expression(NULL), leftExp(_leftExp), rightExp(_rightExp), op(_op) {
            if (leftExp->isBoolean() && rightExp->isBoolean()) {
                this->type = new BooleanType();
                BoolOp b = _op->op;
                switch (b) {
                    case And:
                        codeBuffer.bpatch(leftExp->trueList,beforeRhsMarker->label);
                        this->trueList=rightExp->trueList;
                        this->falseList=codeBuffer.merge(leftExp->falseList,rightExp->falseList);
                        break;
                    case Or:
                        codeBuffer.bpatch(leftExp->falseList,beforeRhsMarker->label);
                        this->falseList=rightExp->falseList;
                        this->trueList=codeBuffer.merge(leftExp->trueList,rightExp->trueList);
                        break;
                }
            } else {
                errorMismatch(yylineno);
                exit(1);
            }

        }

    private:
        ReturnType *getLargerType() {
            if (isSameType<ByteType>(leftExp->type, rightExp->type)
                || isSameType<IntType>(leftExp->type, rightExp->type)) {
                return leftExp->type->clone();
            } else {
                return new IntType();
            }
        }

    public:

        virtual ~BinaryExpression() {

            delete leftExp;
            delete rightExp;
            delete op;

        }
    };

    class UnaryExpression : public Expression {
    public:
        explicit UnaryExpression(ReturnType *_type) : Expression(_type) {}

        virtual ~UnaryExpression() = default;
    };

    class Not : public UnaryExpression {
    public:
        Expression *exp;

        explicit Not(Expression *_exp) : UnaryExpression(new BooleanType()), exp(_exp) {
            if (!exp->isBoolean()) {
                errorMismatch(yylineno);
                exit(1);
            }

            trueList = exp->falseList;
            falseList = exp->trueList;
        }

        Id *isPreconditionable() {
            return NULL;
        }

        virtual ~Not() {

            delete (exp);

        };
    };


    class Multiplicative : public BinaryOperation {
    public:
        explicit Multiplicative(string text) : BinaryOperation(text) {}

        virtual ~Multiplicative() = default;
    };

    class Additive : public BinaryOperation {
    public:
        explicit Additive(string text) : BinaryOperation(text) {}

        virtual ~Additive() = default;
    };


    class RelationalOperation : public Relop {
    public:

        explicit RelationalOperation(string text) : Relop(text) {}

        virtual ~RelationalOperation() = default;
    };

    class EqualityOperation : public Relop {
    public:

        explicit EqualityOperation(string text) : Relop(text) {}

        virtual ~EqualityOperation() = default;
    };


    class Boolean : public UnaryExpression {
    public:
        bool value;

        explicit Boolean(bool val) : UnaryExpression(new BooleanType()), value(val) {
            if (val) {
                trueList = codeBuffer.makelist(assembler.j());
            } else {
                falseList = codeBuffer.makelist(assembler.j());
            }
        }

        Id *isPreconditionable() {
            return NULL;
        }

        virtual ~Boolean() {

        }
    };

    class Id : public UnaryExpression {
    public:
        string name;
        int offset;
        IdentifierType idType;

        friend bool operator==(const Id &, const Id &);

        friend bool operator!=(const Id &, const Id &);

        explicit Id(string text) : UnaryExpression(new Type()), name(text), idType(VariableType) {}

        Id(string text, ReturnType *_type, IdentifierType _idType) : UnaryExpression(_type),
                                                                     name(text), idType(_idType) {}

        explicit Id(Id *id) : UnaryExpression(id->type->clone()) {
            name = id->name;
            offset = id->offset;
            idType = id->idType;
        }

        Id *isPreconditionable() {
            if (offset < 0)return NULL;
            return this;
        }

        Id *changeIdTypeToFunction() {
            idType = FunctionType;
            return this;
        }

        Id *updateType(Type *t) {
            delete this->type;
            this->type = t;
            return this;
        }

        bool isFunction() {
            return idType == FunctionType;
        }

        virtual ~Id() = default;
    };


    class String : public UnaryExpression {
    public:
        string value;
        string label;

        explicit String(string text) : UnaryExpression(new StringType()), value(text), label("") {
            label = CodeBuffer::instance().genLabel();
            assembler.emitStringToData(label, value);
            registerName = registers.regAlloc();
            assembler.la(registerName, label);

        }


        Id *isPreconditionable() {
            return NULL;
        }

        virtual ~String() = default;
    };

    class Number : public UnaryExpression {
    public:
        int value;

        Number(string text, Type *_type) : UnaryExpression(_type), value(atoi(text.c_str())) {}

        Number(int val, Type *_type) : UnaryExpression(_type), value(val) {
            registerName = registers.regAlloc();
            assembler.li(registerName, val);
        }

        Id *isPreconditionable() {
            return NULL;
        }

        virtual ~Number() {

        }

    };

    class Integer : public Number {
    public:
        explicit Integer(Number *n) : Number(n->value, new IntType()) {
            delete n;

        }

        virtual  ~Integer() {

        }
    };

    class Byte : public Number {
    public:
        explicit Byte(Number *num) : Number(num->value, new ByteType()) {


            delete num; //check
            if (value > 255) {
                stringstream stream;
                stream << value;
                errorByteTooLarge(yylineno, stream.str());
                exit(1);
            }

        }

        virtual  ~Byte() = default;
    };

    class FormalDec : public Node {
    public:
        Type *type;
        Id *id;

        FormalDec(Type *_type, Id *_id) : type(_type), id(_id) {

        }

        virtual ~FormalDec() {

            delete type;

        }


    };

    class FormalList : public Node {
    public:
        vector<FormalDec *> decelerations;

        FormalList() = default;

        explicit FormalList(FormalDec *formalDec) {

            add(formalDec);

        }

        FormalList *add(FormalDec *formalDec) {
            decelerations.insert(decelerations.begin(), formalDec);
            return this;
        }

        int size() { return decelerations.size(); }

        virtual ~FormalList() = default;
    };

    class PreCondition : public Node {
    public:
        Expression *exp;

        explicit PreCondition(Expression *_exp) : exp(_exp) {

        }

        Id *isExpressionValid() {

            return exp->isPreconditionable();
        }

        virtual ~PreCondition() {

            delete exp;

        }
    };

    class PreConditions : public Node {
    public:
        vector<PreCondition *> preconditions;

        PreConditions() = default;

        PreConditions *add(PreCondition *precond) {

            preconditions.insert(preconditions.begin(), precond);

            return this;
        }

        int size() {
            return preconditions.size();
        }

        Id *isValid() {

            vector<PreCondition *>::iterator it = preconditions.begin();
            while (it != preconditions.end()) {
                Id *i = (*it)->isExpressionValid();
                if (i != NULL)
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

        explicit ExpressionList(Expression *exp) {
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

        friend bool operator==(const FuncDec &, const FuncDec &);

        friend bool operator!=(const FuncDec &, const FuncDec &);


        FuncDec(ReturnType *_returnType,
                Id *_id, FormalList *_arguments,
                PreConditions *_conditions) : returnType(_returnType),
                                              id(_id),
                                              arguments(_arguments),
                                              conditions(_conditions) {}


        vector<string> *getArgsAsString() {
            vector<FormalDec *>::iterator it = this->arguments->decelerations.begin();
            vector<string> *typesName = new vector<string>();
            while (it != arguments->decelerations.end()) {
                typesName->push_back((*it)->type->typeName());
                ++it;
            }
            return typesName;
        }


        bool isArgumentListMatch(ExpressionList *expList) {

            vector<Expression *>::iterator expIt = expList->expressions.begin();
            vector<FormalDec *>::iterator formalIt = this->arguments->decelerations.begin();
            while ((expIt != expList->expressions.end()) && (formalIt != arguments->decelerations.end())) {
                Expression *currentExp = *expIt;
                FormalDec *currentArgument = *formalIt;
                if (!currentArgument->type->canBeAssigned(currentExp->type))
                    return false;

                ++expIt;
                ++formalIt;
            }
            if (expIt == expList->expressions.end() && formalIt == arguments->decelerations.end())
                return true;
            return false;

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
                : UnaryExpression(_returnType->clone()), id(_id), expressions(_expressions) {
            assembler.comment("call to function - saving regs");
            vector<string> used = registers.getUsedRegisters();
            saveAndFreeRegs(used);
            assembler.subu("$sp", "$sp", WORD_SIZE * 2);
            assembler.sw("$fp", WORD_SIZE, "$sp");
            assembler.sw("$ra", 0, "$sp");
            int argsSize = expressions->expressions.size();
            assembler.subu("$sp", "$sp", WORD_SIZE * argsSize);
            for (int i = 0; i < argsSize; i++) {
                string &reg = expressions->expressions[i]->registerName;
                assembler.sw(reg, i * WORD_SIZE, "$sp");
                Registers::getInstance().regFree(reg);
            }
            assembler.comment("jump to function - " + id->name);
            assembler.jal(id->name);
            assembler.comment("return from functionn  - " + id->name + "restoring the regs");
            assembler.addu("$sp", "$sp", WORD_SIZE * argsSize);
            assembler.lw("$ra", 0, "$sp");
            assembler.lw("$fp", WORD_SIZE, "$sp");
            assembler.addu("$sp", "$sp", WORD_SIZE * 2);
            restoreRegs(used);
            assembler.comment("end Call");
        }

        Id *isPreconditionable() {

            vector<Expression *>::iterator it = expressions->expressions.begin();
            while (it != expressions->expressions.end()) {
                Id *i = (*it)->isPreconditionable();
                if (NULL != i)
                    return i;
                ++it;
            }
            return NULL;
        }

        virtual ~Call() {

            delete expressions;
            delete id;

        }

    private:

        void restoreRegs(vector<string> &regs) {
            assembler.comment("restore all used regs");
            for (int i = 0; i < regs.size(); i++) {
                assembler.lw(regs[i], WORD_SIZE * i, "$sp");
                registers.markAsUsed(regs[i]);
            }
            assembler.addu("$sp", "$sp", regs.size() * WORD_SIZE);
        }

        void saveAndFreeRegs(vector<string> &regs) {
            assembler.comment("save all used regs");
            assembler.subu("$sp", "$sp", regs.size() * WORD_SIZE);
            for (int i = 0; i < regs.size(); i++) {
                assembler.sw(regs[i], WORD_SIZE * i, "$sp");
                registers.regFree(regs[i]);
            }

        }
    };

    class Scope {
    public:
        vector<Id *> variables;
        vector<FuncDec *> functions;
        Scope *parent;

        Scope() : variables(), functions(), parent(NULL) {
        }

        explicit Scope(Scope *_parent) : parent(_parent) {}


        void addVariable(Id *id) {

            variables.push_back(id);
        }

        void addFunction(FuncDec *func) {

            func->id->changeIdTypeToFunction();
            functions.push_back(func);
            Id *i = new Id(func->id);
            variables.push_back(i);

        }

    private:
        bool isFunctionExistInScope(FuncDec *func) {

            vector<FuncDec *>::iterator it = functions.begin();
            while (it != functions.end()) {
                FuncDec *currentFunc = *it;
                if (*func == *currentFunc)
                    return true;
                ++it;
            }
            return false;
        }


        FuncDec *getFunctionInScope(Id *id) {

            for (vector<FuncDec *>::iterator it = functions.begin(); it != functions.end(); ++it) {
                FuncDec *currentFunc = *it;
                if (*currentFunc->id == *id) {
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
        bool isFunctionExist(FuncDec *func) {
            if (isFunctionExistInScope(func)) {
                return true;
            } else if (NULL != parent) {
                return parent->isFunctionExist(func);
            } else {
                return false;
            }
        }

        virtual FuncDec *getFunction(Id *id) {

            FuncDec *found = getFunctionInScope(id);
            if (NULL != found) {
                return found;
            } else if (NULL != parent) {
                return parent->getFunction(id);
            } else {
                return NULL;
            }
        }

    private:
        Id *getVariableInScope(Id *id) {

            vector<Id *>::iterator it = variables.begin();
            while (it != variables.end()) {
                Id *currentId = *it;
                if (*id == *currentId)
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
        Id *getVariable(Id *id) {

            Id *found = getVariableInScope(id);
            if (NULL != found) {
                return found;
            } else if (NULL != parent) {
                return parent->getVariable(id);
            } else {
                return NULL;
            }
        }

        void printIds() {

            for (vector<Id *>::iterator it = variables.begin(); it != variables.end(); ++it) {
                if ((*it)->isFunction()) {
                    FuncDec *fun = getFunction(*it);
                    vector<string> *args = fun->getArgsAsString();
                    string func_type = makeFunctionType(fun->returnType->typeName(), *args);
                    delete args;
                    //output::printID((*it)->name, 0, func_type);
                } else {
                    //output::printID((*it)->name, (*it)->offset, (*it)->type->typeName());
                }
            }
        }


        virtual void endScope() {

            //output::endScope();
            printIds();


        }

        virtual ~Scope() {

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
        explicit WhileScope(Scope *_parent) : Scope(_parent) {}

        virtual ~WhileScope() = default;
    };

    class FunctionScope : public Scope {
    public:
        FuncDec *func;

        explicit FunctionScope(Scope *_parent) : Scope(_parent), func(NULL) {}

        void updateFunctionScope(FuncDec *_func) {
            func = _func;
            if (NULL != this->parent)
                parent->addFunction(_func);
        }

        void updateFunctionScope(PreConditions *preconditions) {
            func->conditions = preconditions;
        }

        FuncDec *getFunction() {
            return func;
        }

        void endScope() {

            //output::endScope();
            //output::printPreconditions(func->id->name, func->conditions->size());
            printIds();

        }

        virtual ~FunctionScope() = default;
    };

}

#endif //_PARSER_H