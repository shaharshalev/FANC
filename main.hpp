#ifndef HW3_MAIN_H
#define HW3_MAIN_H
#include "parser.hpp"
#include "output.hpp"
#include <assert.h>     /* assert */

#define PRINT_LABEL "print"
#define PRINTI_LABEL "printi"

namespace FanC {
    void reduceProgram();
    void reduceFuncDecl(Id* id);
    void reduceFuncDeclSignature(ReturnType* returnType,Id* id,FormalList* formals);
    void reducePreConditionsDecl(PreConditions* preconditions);
    void reduceStatement();
    void handleWhile();
    void reduceFormalDecl(Type* type,Id* id);
    void reduceOpenWhileScope(Expression *exp);
    void reduceOpenScope();
    void reduceOpenFunctionScope();
    void reduceEndScope();
    FormalList* reduceFormalsList(FormalList* formalList,FormalDec* formalDec);
    PreConditions *reducePreConditions(PreConditions *preConditions,PreCondition* precondition);

    void validateExpIsBool(Expression *exp);

    void validateWhile(WhileOp op);

    bool inWhile();

    void validateFunctionReturnType(Expression *exp);

    void validateAssignment(Id *id, Expression *exp);

    void insertVarToTable(Id *id);

    void handleTypeDecl(Type *type, Id *id);

    Id *extractIdFromSymbolTable(Id *id);

    void assignToVar(Id *id, Expression *exp);

    void reduceOpenIfScope(Expression *exp);

    FuncDec *getFunction();

    void handleArgumentDecl(FormalList *formalList);

    Call *handleCall(Id *id, ExpressionList *expList);

    void assertIdentifierNotExists(Id *id);

    void handleIDExpression(Id *id);

    string getRegister(Expression* exp);

    void saveReturnValueInCallRegister(Call *call);

    void handleRegisterInAssignmentDecl(Expression *exp);

    void initVariableInStack();

    void changeBranchToVar(Expression* exp);

    void checkAndNotifyIfMain(Id *id, FormalList *formals, ReturnType *returnType);

    int yyerror(const char * message);

    void initProgramHeader();

    void jumpToCaller();

    void updateReturnReg(Expression* exp);

    void jumpFromBreak(Node* node);

    void jumpFromContinue(Node* node);

    void mergeLists(Node* destNode, Node* srcNode);

    void funDecInAssembly(Id* id);


}
#endif //HW3_MAIN_H
