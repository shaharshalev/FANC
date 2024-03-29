#ifndef HW3_MAIN_H
#define HW3_MAIN_H
#include "parser.hpp"
#include "output.hpp"
#include <assert.h>     /* assert */

#define PRINT_LABEL "print"
#define PRINTI_LABEL "printi"

#define AFTER_PRECOND_PREFIX_LABEL "after_precond_"
namespace FanC {
    void reduceProgram();
    void reduceFuncDecl(FuncDec *funDec, Expression *tempExp, Statements *statements);
    FuncDec * reduceFuncDeclSignature(ReturnType* returnType, Id* id, FormalList* formals);
    Expression * reducePreConditionsDecl(PreConditions *preconditions);
    void reduceStatement();
    Statement * handleWhile(M *beforeConditionMarker, Expression *exp, M *beforeStatementMarker, Statement *statement,
                            M *endWhileMarker);
    void reduceFormalDecl(Type* type,Id* id);
    void reduceOpenWhileScope(Expression *exp);
    void reduceOpenScope();
    void reduceOpenFunctionScope();
    int reduceEndScope();
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

    Statement* jumpFromBreak();
    Statement* jumpFromContinue();


    void mergeLists(Statement* dest, Statement* src);

    void funDecInAssembly(Id* id);

    /**
     * the function assemble new statement with the continueList and breakList of the [statement]
     * and backpatch the trueList with trueMarker, and falseList with falseMarker
     * @param exp
     * @param trueMarker
     * @param statement
     * @return new statement
     */
    Statement *assembleIf(Expression *exp, M *trueMarker, Statement *statement);

    Statement *assembleIfElse(Expression *exp, M *trueMarker, N *skipElse, M *falseMarker, Statement *trueStatement,
                              Statement *falseStatement);

    void foldScope();

    Statement* assembleStatements(Statements* statements);

}
#endif //HW3_MAIN_H
