#ifndef _AST_CREATE_DOC_H_
#define _AST_CREATE_DOC_H_

#include "ASTNode.h"
#include "AST.h"

class ASTCreateDoc : public ASTNode
{
public:
    ASTNode *doc, *coll;

public:
    ASTCreateDoc(ASTLocation &loc, ASTNode *doc_, ASTNode *coll_ = NULL) : ASTNode(loc), doc(doc_), coll(coll_) {}

    ~ASTCreateDoc();

    void accept(ASTVisitor &v);
    ASTNode *dup();
};

#endif
