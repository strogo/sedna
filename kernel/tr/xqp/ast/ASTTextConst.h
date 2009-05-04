#ifndef _AST_TEXT_CONST_H_
#define _AST_TEXT_CONST_H_

#include "ASTNode.h"
#include "AST.h"

class ASTTextConst : public ASTNode
{
public:
    ASTNode *expr; // computed construction expression

public:
    ASTTextConst(ASTLocation &loc, ASTNode *expr_) : ASTNode(loc), expr(expr_) {}

    ~ASTTextConst();

    void accept(ASTVisitor &v);
    ASTNode *dup();
};

#endif
