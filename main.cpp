#include <iostream>
#include <stdlib.h>
#include "main.hpp"
#include <assert.h>     /* assert */
using namespace std;

namespace FanC {
    vector<Scope *> symbolTable;
    vector<int> offsets;
    bool isMainExist = false;

    void reduceProgram() {
        if (!isMainExist) {
            errorMainMissing();
            exit(1);
        }
        symbolTable.back()->endScope();
    }


    void validateMain(Id *id, FormalList *formals, ReturnType *returnType) {
        if (id->name == "main") {
            if (returnType->typeName() != "VOID" || formals->size() != 0) {
                errorMismatch(yylineno);
                exit(1);
                //todo validate error
            }
            if (isMainExist) {
                errorDef(yylineno, "main");
                exit(1);
            }
            isMainExist = true;
        }

    }

    void validateWhile(WhileOp op) {
        if (inWhile()) {
            switch (op) {
                case Break:
                    errorUnexpectedBreak(yylineno);
                    break;
                case Continue:
                    errorUnexpectedContinue(yylineno);
                    break;
            }
        }
    }

    void validateExpIsBool(Expression *exp) {
        if (!isInstanceOf<BooleanType>(exp->type)) {
            errorMismatch(yylineno);
            exit(1);
        }
    }

    void handleWhile(Expression *exp) {
        validateExpIsBool(exp);
        symbolTable.back()->endScope();
        delete exp;
    }

    bool inWhile() {
        for (vector<Scope *>::reverse_iterator i = symbolTable.rbegin(); i != symbolTable.rend(); ++i) {
            Scope *scope = *i;
            if (isInstanceOf<WhileScope>(scope)) return true;
        }
        return false;
    }

/**
* The function returns the function of the scope that we are in.
* null is returned if we are not in a function.
*/
    FuncDec *getFunction() {
        for (vector<Scope *>::reverse_iterator i = symbolTable.rbegin(); i != symbolTable.rend(); ++i) {
            Scope *scope = *i;
            if (isInstanceOf<FunctionScope>(scope)) {
                FunctionScope *functionScope = dynamic_cast<FunctionScope *>(scope);
                return functionScope->getFunction();
            }
        }
        return NULL;
    }


    /**
    * this function validates that we return from a function
    * and the expression is of the same type as the function returned type.
    */
    void validateFunctionReturnType(Expression *exp) {
        FuncDec *func = getFunction();
        if ((NULL == func)
            || (exp == NULL && !isInstanceOf<Void>(func->returnType))
            || (exp != NULL && (exp->type->typeName() != func->returnType->typeName()))) {
            errorMismatch(yylineno);
            exit(1);
        }
        if (exp != NULL)
            delete exp;
    }

    Call *handleCall(Id *id, ExpressionList *expList) {
        FuncDec *func = symbolTable.back()->getFunction(id);

        if (NULL == func) {
            errorUndefFunc(yylineno, id->name);
            exit(1);
        }

        if (!func->isArgumentListMatch(expList)) {
            vector<string> *strVec = func->getArgsAsString();
            vector<string> &v = *strVec;
            errorPrototypeMismatch(yylineno, id->name, v);
            delete strVec;
            exit(1);
        }
        return new Call(func->returnType, id, expList);
    }

    /*
    **this function validate assignment to id by comparing exp type.
    ** this function assume that id type is updated.
    */
    void validateAssignment(Id *id, Expression *exp) {
        if (id->type->typeName() != exp->type->typeName()) {
            errorMismatch(yylineno);
            exit(1);
        }

    }

    void assertIdentifierNotExists(Id *id) {
        Scope *scope = symbolTable.back();
        if (scope->getVariable(id) != NULL) {
            errorDef(yylineno, id->name);
            exit(1);
        }
    }

    void insertVarToTable(Id *id) {
        assertIdentifierNotExists(id);
        int newOffset = offsets.back();
        offsets.pop_back();
        offsets.push_back(newOffset + 1);
        id->offset = newOffset;
        symbolTable.back()->addVariable(id);
    }

    void handleTypeDecl(Type *type, Id *id) {
        id->updateType(type);
        insertVarToTable(id);

    }

    void handleArgumentDecl(FormalList *formalList) {

        int offset = -1;
        vector<FormalDec *>::iterator it = formalList->decelerations.begin();
        while (formalList->decelerations.end() != it) {
            FormalDec *formalDec = *it;
            Id *id = new Id(formalDec->id);
            id->offset = offset;
            id->type=formalDec->type;
            symbolTable.back()->addVariable(id);
            --offset;
            ++it;
        }
    }

    /**
    ** This function extracts the id from symbol table.
    ** if id does not exist the function will exit with error
    */
    Id *extractIdFromSymbolTable(Id *id) {
        Id *i = symbolTable.back()->getVariable(id);
        if (NULL == i) {
            errorUndef(yylineno, id->name);
            exit(1);
        }
        return i;
    }

    void handleIDExpression(Id *id) {
        Id *idFromSymbolTable = extractIdFromSymbolTable(id);
        id->type= idFromSymbolTable->type;
        id->offset = idFromSymbolTable->offset;
    }

    void assignToVar(Id *id, Expression *exp) {
        Id *idFromSymbolTable = extractIdFromSymbolTable(id);
        if (idFromSymbolTable->isFunction()) {
            errorUndef(yylineno, id->name);
            exit(1);
        }
        validateAssignment(idFromSymbolTable, exp);
        delete id;
        delete exp;
    }

    void handleIf(Expression *exp) {
        //TODO: fix that a var of bool type is also of type BoolType
        validateExpIsBool(exp);
        symbolTable.back()->endScope();
        delete exp;
    }


    bool operator==(const Id &a, const Id &b) {
        return a.name == b.name;
    }

    bool operator!=(const Id &a, const Id &b) {
        return a.name != b.name;
    }

    bool operator==(const FuncDec &a, const FuncDec &b) {
        return a.id == b.id;
    }

    bool operator!=(const FuncDec &a, const FuncDec &b) {
        return a.id != b.id;
    }

    void reduceFuncDecl() {
        symbolTable.back()->endScope();
    }

    void reduceFuncDeclSignature(ReturnType *returnType, Id *id, FormalList *formals) {
        assertIdentifierNotExists(id);
        validateMain(id, formals, returnType);
        FunctionScope *functionScope = dynamic_cast<FunctionScope *>(symbolTable.back());
        functionScope->updateFunctionScope(new FuncDec(returnType, id, formals, NULL));
    }

    void reducePreConditionsDecl(PreConditions *preconditions) {
        Id *i = preconditions->isValid();
        if (i != NULL) {
            errorUndef(yylineno, i->name);
            exit(1);
        }
        FunctionScope *functionScope = dynamic_cast<FunctionScope *>(symbolTable.back());
        functionScope->updateFunctionScope(preconditions);
    }

    void reduceFormalDecl(Type *type, Id *id) {
        id->updateType(type);
    }

    void reduceOpenWhileScope() {
        symbolTable.push_back(new WhileScope(symbolTable.back()));
        offsets.push_back(offsets.back());
    }

    void reduceOpenScope() {
        if (symbolTable.empty()) {
            assert(offsets.empty());
            symbolTable.push_back(new Scope(NULL));
            offsets.push_back(0);
            FormalList *printArguments = new FormalList(new FormalDec(new StringType(), NULL));
            FuncDec *printDec = new FuncDec(new Void(), new Id("print", new Void(), FunctionType), printArguments,
                                            NULL);
            symbolTable.back()->addFunction(printDec);
            FormalList *printIArguments = new FormalList(new FormalDec(new IntType(), NULL));
            FuncDec *printIDec = new FuncDec(new Void(), new Id("printi", new Void(), FunctionType), printIArguments,
                                             NULL);
            symbolTable.back()->addFunction(printIDec);
        } else {
            symbolTable.push_back(new Scope(symbolTable.back()));
            offsets.push_back(offsets.back());
        }
    }

    PreConditions *reducePreConditions(PreConditions *preConditions, PreCondition *precondition) {
        return preConditions->add(precondition);
    }

    void reduceOpenFunctionScope() {
        symbolTable.push_back(new FunctionScope(symbolTable.back()));
        offsets.push_back(offsets.back());
    }

    void reduceEndScope() {
        Scope *currentScope = symbolTable.back();
        symbolTable.pop_back();
        offsets.pop_back();
        currentScope->endScope();
        delete currentScope;
    }

    void reduceStatement() {
        symbolTable.back()->endScope();
    }


}
