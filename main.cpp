#include <iostream>
#include <stdlib.h>
#include "main.hpp"
#include <assert.h>     /* assert */
#include "registers.hpp"
#include "bp.hpp"
#include "assembler_coder.hpp"
using namespace std;

namespace FanC {
    vector<Scope *> symbolTable;
    vector<int> offsets;
    bool isMainExist = false;




    void checkAndNotifyIfMain(Id *id, FormalList *formals, ReturnType *returnType) {

        if (id->name == "main") {
            if (returnType->typeName() != Void().typeName() || formals->size() != 0) {
                return; //for now we just ignore a function that was not declared, but with the wrong type
            }

            isMainExist = true;
        }

    }

    void validateWhile(WhileOp op) {

        if (!inWhile()) {
            switch (op) {
                case Break:
                    errorUnexpectedBreak(yylineno);
                    exit(1);
                case Continue:
                    errorUnexpectedContinue(yylineno);
                    exit(1);
            }
        }
    }

    void validateExpIsBool(Expression *exp) {

        if (!isInstanceOf<BooleanType>(exp->type)) {
            errorMismatch(yylineno);
            exit(1);
        }
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
            || (exp != NULL && (!func->returnType->canBeAssigned(exp->type)||isInstanceOf<Void>(func->returnType) ))) {
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

        if (!id->type->canBeAssigned(exp->type)) {
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
            assertIdentifierNotExists(formalDec->id);
            Id *id = new Id(formalDec->id);
            id->offset = offset;
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
        id->registerName = getRegister(id);
        id->type= idFromSymbolTable->type->clone();
        id->offset = idFromSymbolTable->offset;
        if(id->isBoolean()){
            int bneAdd = AsssemblerCoder::getInstance().bne("$0",id->registerName);
            id->trueList = CodeBuffer::instance().makelist(bneAdd);
            id->falseList = CodeBuffer::instance().makelist(AsssemblerCoder::getInstance().j());
        }
    }

    void saveRegisterToStack(Id *id, Expression *exp) {
        changeBranchToVar(exp);//must be before saving to stack
        AsssemblerCoder::getInstance().sw(exp->registerName,id->offset*(-WORD_SIZE),"$fp");
        Registers::getInstance().regFree(exp->registerName);
    }


    void assignToVar(Id *id, Expression *exp) {

        Id *idFromSymbolTable = extractIdFromSymbolTable(id);
        if (idFromSymbolTable->isFunction()) {
            errorUndef(yylineno, id->name);
            exit(1);
        }
        validateAssignment(idFromSymbolTable, exp);

        saveRegisterToStack(idFromSymbolTable,exp);
        delete id;
        delete exp;
    }


    void reduceOpenIfScope(Expression *exp) {

        symbolTable.push_back(new Scope(symbolTable.back()));
        offsets.push_back(offsets.back());
        validateExpIsBool(exp);
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

        reduceEndScope();
    }

    void reduceFuncDeclSignature(ReturnType *returnType, Id *id, FormalList *formals) {

        assertIdentifierNotExists(id);
        checkAndNotifyIfMain(id, formals, returnType);
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

    void reduceOpenWhileScope(Expression* exp) {

        validateExpIsBool(exp);
        symbolTable.push_back(new WhileScope(symbolTable.back()));
        offsets.push_back(offsets.back());
        delete exp;
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

        reduceEndScope();
    }



    FormalList *reduceFormalsList(FormalList *formalList, FormalDec *formalDec) {

        return formalList->add(formalDec);
    }

    void reduceProgram() {
        if (!isMainExist) {
            errorMainMissing();
            exit(1);
        }
        reduceEndScope();
    }

    void handleWhile() {

        reduceEndScope();
    }

    int yyerror(const char * message){
        errorSyn(yylineno);
        exit(1);
    }

    string getRegister(Expression *exp) {
        if(exp->registerName!=""){
            return exp->registerName;
        }
        Id* id = extractIdFromSymbolTable((Id*)exp);
        string regName = Registers::getInstance().regAlloc();
        /* the loading from frame pointer will work only if there will
         * since there are no global variables */
        AsssemblerCoder::getInstance().lw(regName,id->offset*-4);
        return regName;
    }

    void divZeroBody(){
        string errorDivZeroLabel="error_div_zero";
        AsssemblerCoder::getInstance().emitStringToData(errorDivZeroLabel+":","Error division by zero\\n");

        CodeBuffer::instance().emit((string)DIV_BY_ZERO_LABEL+":");
        string regName = Registers::getInstance().regAlloc();
        AsssemblerCoder::getInstance().la(regName,errorDivZeroLabel);
        /*prepering stack for function print*/
        AsssemblerCoder::getInstance().subu("$sp","$sp",WORD_SIZE*3); //make space for 3 reg alloc
        AsssemblerCoder::getInstance().sw("$fp",2*WORD_SIZE,"$sp");
        AsssemblerCoder::getInstance().sw("$ra",WORD_SIZE,"$sp");
        AsssemblerCoder::getInstance().sw(regName,0,"$sp");
        AsssemblerCoder::getInstance().subu("$fp","$sp",WORD_SIZE);
        /*jumping to print function*/
        Registers::getInstance().regFree(regName);
        AsssemblerCoder::getInstance().jal(PRINT_LABEL);
        /*restore the reg from stack */
        AsssemblerCoder::getInstance().lw("$ra",WORD_SIZE,"$sp");
        AsssemblerCoder::getInstance().lw("$fp",2*WORD_SIZE,"$sp");
        AsssemblerCoder::getInstance().addu("$sp","$sp",WORD_SIZE*3);
        AsssemblerCoder::getInstance().exitSyscall();
    }

    void printBody(){
        AsssemblerCoder::getInstance().printSyscall(WORD_SIZE);
        AsssemblerCoder::getInstance().jr();
    }

    void printiBody(){
        AsssemblerCoder::getInstance().printiSyscall(WORD_SIZE);
        AsssemblerCoder::getInstance().jr();
    }

    void initProgramHeader() {

        divZeroBody();
        printBody();
        printiBody();

    }

    void saveReturnValueInCallRegister(Call *call) {
        call->registerName=Registers::getInstance().regAlloc();
        AsssemblerCoder::getInstance().move(call->registerName,"$v0");
    }

    void handleRegisterInAssignmentDecl(Expression *exp) {
        changeBranchToVar(exp);//must be first
        AsssemblerCoder::getInstance().subu("$sp","$sp",WORD_SIZE);
        AsssemblerCoder::getInstance().sw(exp->registerName,0,"$sp");
        Registers::getInstance().regFree(exp->registerName);
    }

    void initVariableInStack() {
        AsssemblerCoder::getInstance().subu("$sp","$sp",WORD_SIZE);
        AsssemblerCoder::getInstance().sw("$0",0,"$sp");
    }

    void changeBranchToVar(Expression *exp) {
        if(!exp->isBoolean()) return;
        string reg=Registers::getInstance().regAlloc();
        string trueLabel=CodeBuffer::instance().genLabel();
        AsssemblerCoder& assembler=AsssemblerCoder::getInstance().getInstance();
        assembler.li(reg,1);
        vector<int> end=CodeBuffer::makelist(assembler.j());

        string falseLabel=CodeBuffer::instance().genLabel();
        assembler.li(reg,0);
        string endLabel=CodeBuffer::instance().genLabel();
        //back-patching:
        CodeBuffer::instance().bpatch(end,endLabel);
        CodeBuffer::instance().bpatch(exp->trueList, trueLabel);
        CodeBuffer::instance().bpatch(exp->falseList, falseLabel);
        exp->registerName=reg;
    }


}
