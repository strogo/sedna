/*
 * File:  PPInsertBefore.cpp
 * Copyright (C) 2004 The Institute for System Programming of the Russian Academy of Sciences (ISP RAS)
 */

#include "common/sedna.h"

#include "tr/executor/root/PPInsertBefore.h"
#include "tr/executor/base/visitor/PPVisitor.h"
#include "tr/updates/updates.h"
#include "tr/locks/locks.h"

PPInsertBefore::PPInsertBefore(PPOpIn _child1_, 
                               PPOpIn _child2_,
                               dynamic_context *_cxt_) : PPUpdate("PPInsertBefore"),
                                                          child1(_child1_),
                                                          child2(_child2_),
                                                          cxt(_cxt_)
{
}

PPInsertBefore::~PPInsertBefore()
{
    delete child1.op;
    child1.op = NULL;
    delete child2.op;
    child2.op = NULL;
    delete cxt;
    cxt = NULL;
}

void PPInsertBefore::do_open()
{
    local_lock_mrg->lock(lm_x);
    cxt->global_variables_open();
    child1.op->open();
    child2.op->open();
}

void PPInsertBefore::do_close()
{
    child1.op->close();
    child2.op->close();
    cxt->global_variables_close();
}

void PPInsertBefore::do_accept(PPVisitor &v)
{
    v.visit (this);
    v.push  (this);
    child1.op->accept(v);
    child2.op->accept(v);
    v.pop();
}

void PPInsertBefore::do_execute()
{
    insert_before(child1, child2);
}
