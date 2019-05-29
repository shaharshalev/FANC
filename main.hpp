#ifndef HW3_MAIN_H
#define HW3_MAIN_H
#include "parser.hpp"
#include "output.hpp"
#include <assert.h>     /* assert */
namespace FanC {
    void reduceProgram();
    void reduceFuncDecl();
    void reduceFuncDeclSignature(ReturnType* returnType,Id* id,FormalList* formals);
    void reducePreConditionsDecl(PreConditions* preconditions);
    void reduceStatement();
    void handleWhile(Expression *exp);
    void reduceFormalDecl(Type* type,Id* id);
    void reduceOpenWhileScope();
    void reduceOpenScope();
    void reduceOpenFunctionScope();
    void reduceEndScope();
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

    void handleIf(Expression *exp);

    FuncDec *getFunction();

    void handleArgumentDecl(FormalList *formalList);

    Call *handleCall(Id *id, ExpressionList *expList);

    void assertIdentifierNotExists(Id *id);

    void handleIDExpression(Id *id);

    void validateMain(Id *id, FormalList *formals, ReturnType *returnType);
}
#endif //HW3_MAIN_H
