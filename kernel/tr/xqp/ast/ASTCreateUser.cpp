/*
 * File:  ASTCreateUser.cpp
 * Copyright (C) 2009 The Institute for System Programming of the Russian Academy of Sciences (ISP RAS)
 */

#include "tr/xqp/visitor/ASTVisitor.h"
#include "ASTCreateUser.h"

ASTCreateUser::~ASTCreateUser()
{
    delete user;
    delete psw;
}

void ASTCreateUser::accept(ASTVisitor &v)
{
    v.visit(*this);
}

ASTNode *ASTCreateUser::dup()
{
    return new ASTCreateUser(loc, new std::string(*user), new std::string(*psw));
}