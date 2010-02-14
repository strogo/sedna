/*
 * File:  PPCreateIndex.cpp
 * Copyright (C) 2004 The Institute for System Programming of the Russian Academy of Sciences (ISP RAS)
 */

#include <string>
 
#include "common/sedna.h"

#include "tr/executor/root/PPCreateIndex.h"
#include "tr/executor/base/PPUtils.h"
#include "tr/executor/base/visitor/PPVisitor.h"
#include "tr/idx/indexes.h"
#include "tr/locks/locks.h"
#include "tr/auth/auc.h"

PPCreateIndex::PPCreateIndex(PPOpIn _index_name_,
                             PathExprRoot _root_,
                             PathExpr *_object_path_,
                             PathExpr *_key_path_,
                             xmlscm_type _key_type_,
                             dynamic_context *_cxt_) : index_name(_index_name_),
                                                       root(_root_),
                                                       object_path(_object_path_),
                                                       key_path(_key_path_),
                                                       key_type(_key_type_),
                                                       cxt(_cxt_)
{
}

PPCreateIndex::~PPCreateIndex()
{
    delete index_name.op;
    index_name.op = NULL;
    
    root.release();
    
    delete cxt;
    cxt = NULL;
}

void PPCreateIndex::do_open()
{
    /* descriptive schema of the document/collection changes */
    local_lock_mrg->lock(lm_x); 
    dynamic_context::global_variables_open();
    index_name.op->open();
    root.open();
}

void PPCreateIndex::do_close()
{
    index_name.op->close();
    root.close();
    dynamic_context::global_variables_close();
}

void PPCreateIndex::do_accept(PPVisitor &v)
{
    v.visit (this);
    v.push  (this);
    index_name.op->accept(v);    
    if(root.get_operation().op != NULL)
    {
        root.get_operation().op->accept(v);
    }
    v.pop();
}

void PPCreateIndex::do_execute()
{
    /* Determine index name */
    tuple_cell tc = get_name_from_PPOpIn(index_name, "index", "create index");

    /* Determine document or collection name to create index on */
    counted_ptr<db_entity> db_ent = root.get_entity("index", "create index"); 

    /* Get xptr on this document or collection*/
    xptr root_obj = get_schema_node(db_ent, (std::string("Unknown document/collection passed to create index: ") + db_ent->name).c_str());

    local_lock_mrg->put_lock_on_index(tc.get_str_mem());
    auth_for_create_index(tc.get_str_mem(), db_ent->name, db_ent->type == dbe_collection);
    create_index(object_path,
                 key_path,
                 key_type,
                 root_obj,
                 tc.get_str_mem(),
                 db_ent->name,
                 (db_ent->type == dbe_document));
}

