/*
 * File:  PPBooleanOps.h
 * Copyright (C) 2004 The Institute for System Programming of the Russian Academy of Sciences (ISP RAS)
 */


#ifndef _PPBOOLEANOPS_H
#define _PPBOOLEANOPS_H

#include "sedna.h"
#include "PPBase.h"

///////////////////////////////////////////////////////////////////////////////
/// PPFnTrue
///////////////////////////////////////////////////////////////////////////////
class PPFnTrue : public PPIterator
{
protected:
    // obtained parameters and local data
    bool first_time;

public:
    virtual void open   ();
    virtual void reopen ();
    virtual void close  ();
    virtual strict_fun res_fun () { return result; };
    virtual void next   (tuple &t);

    virtual PPIterator* copy(variable_context *_cxt_);
    static bool result(PPIterator* cur, variable_context *cxt, void*& r);

    PPFnTrue(variable_context *_cxt_);
    virtual ~PPFnTrue();
};


///////////////////////////////////////////////////////////////////////////////
/// PPFnFalse
///////////////////////////////////////////////////////////////////////////////
class PPFnFalse : public PPIterator
{
protected:
    // obtained parameters and local data
    bool first_time;

public:
    virtual void open   ();
    virtual void reopen ();
    virtual void close  ();
    virtual strict_fun res_fun () { return result; };
    virtual void next   (tuple &t);

    virtual PPIterator* copy(variable_context *_cxt_);
    static bool result(PPIterator* cur, variable_context *cxt, void*& r);

    PPFnFalse(variable_context *_cxt_);
    virtual ~PPFnFalse();
};


///////////////////////////////////////////////////////////////////////////////
/// PPFnNot
///////////////////////////////////////////////////////////////////////////////
class PPFnNot : public PPIterator
{
protected:
    // obtained parameters and local data
    PPOpIn child;
    bool first_time;
    bool eos_reached;

    void children(PPOpIn &_child_) { _child_ = child; }

public:
    virtual void open   ();
    virtual void reopen ();
    virtual void close  ();
    virtual strict_fun res_fun () { return result; };
    virtual void next   (tuple &t);

    virtual PPIterator* copy(variable_context *_cxt_);
    static bool result(PPIterator* cur, variable_context *cxt, void*& r);

    PPFnNot(variable_context *_cxt_, 
            PPOpIn _child_);
    virtual ~PPFnNot();
};

///////////////////////////////////////////////////////////////////////////////
/// PPFnBoolean
///////////////////////////////////////////////////////////////////////////////
class PPFnBoolean : public PPIterator
{
protected:
    // obtained parameters and local data
    bool first_time;
    bool eos_reached;

    PPOpIn child;

public:
    virtual void open   ();
    virtual void reopen ();
    virtual void close  ();
    virtual strict_fun res_fun () { return result; };
    virtual void next   (tuple &t);

    virtual PPIterator* copy(variable_context *_cxt_);
    static bool result(PPIterator* cur, variable_context *cxt, void*& r);

    PPFnBoolean(variable_context *_cxt_,
				PPOpIn _child_);
    virtual ~PPFnBoolean();
};


#endif
