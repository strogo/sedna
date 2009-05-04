#include "tr/xqp/visitor/ASTVisitor.h"
#include "ASTMainModule.h"

ASTMainModule::~ASTMainModule()
{
    delete prolog;
    delete query;
}

void ASTMainModule::setVersionDecl(ASTNode *vd)
{
    prolog->addVersionDecl(vd);
}

void ASTMainModule::accept(ASTVisitor &v)
{
    v.visit(*this);
}

ASTNode *ASTMainModule::dup()
{
    return new ASTMainModule(loc, static_cast<ASTProlog *>(prolog->dup()), static_cast<ASTQuery *>(query->dup()));
}
