/*
 * File:  ASTModuleDecl.h
 * Copyright (C) 2009 The Institute for System Programming of the Russian Academy of Sciences (ISP RAS)
 */

#ifndef _AST_MODULE_DECL_H_
#define _AST_MODULE_DECL_H_

#include "ASTNode.h"
class ASTVisitor;

#include <string>

class ASTModuleDecl : public ASTNode
{
public:
    std::string *name, *uri;

public:
    ASTModuleDecl(const ASTNodeCommonData &loc, std::string *mod_name, std::string *mod_uri) : ASTNode(loc), name(mod_name), uri(mod_uri) {}

    ~ASTModuleDecl();

    void accept(ASTVisitor &v);

    ASTNode *dup();
    void modifyChild(const ASTNode *oldc, ASTNode *newc);

    static ASTNode *createNode(scheme_list &sl);
};

#endif
