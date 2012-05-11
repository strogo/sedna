#ifndef _OPERATIONS_H
#define _OPERATIONS_H

#include "tr/opt/OptTypes.h"
#include "tr/opt/phm/PhysicalModel.h"
#include "tr/opt/alg/Predicates.h"

struct Statistics;

class BinaryOpPrototype : public POProt {
public:
    BinaryOpPrototype(const prot_info_t* pinfo, const POProtIn & _left, const POProtIn & _right)
      : POProt(pinfo) { in.push_back(_left); in.push_back(_right); };
};

class SortMergeJoinPrototype : public BinaryOpPrototype {
protected:
    const Comparison cmp;

    bool needLeftSort;
    bool needRightSort;
public:
    SortMergeJoinPrototype(PhysicalModel * model, const POProtIn & _left, const POProtIn & _right, const Comparison& _cmp);

    virtual void evaluateCost(CostModel* model);
    virtual phop::IOperator * compile();
};

class StructuralJoinPrototype : public BinaryOpPrototype {
    pe::Path path;

    bool needLeftSort;
    bool needRightSort;
protected:
    virtual IElementProducer* __toXML(IElementProducer* ) const;
public:
    StructuralJoinPrototype(PhysicalModel * model, const POProtIn & _left, const POProtIn & _right, const pe::Path& _path);

    virtual void evaluateCost(CostModel* model);
    virtual phop::IOperator * compile();
};

class AbsPathScanPrototype : public POProt {
    DataRoot dataRoot;
    pe::Path path;
protected:
    virtual IElementProducer* __toXML(IElementProducer* ) const;
public:
    bool wantSort;

    const DataRoot & getRoot() const { return dataRoot; };
    const pe::Path & getPath() const { return path; };
    
    AbsPathScanPrototype(PhysicalModel * model, const TupleRef & tref);

    virtual void evaluateCost(CostModel* model);
    virtual phop::IOperator * compile();
};

class PathEvaluationPrototype : public POProt {
    pe::Path path;
protected:
    virtual IElementProducer* __toXML(IElementProducer* ) const;
public:
    PathEvaluationPrototype(PhysicalModel * model, const POProtIn & _left, const TupleRef & _right, const pe::Path& _path);

    virtual void evaluateCost(CostModel* model);
    virtual phop::IOperator * compile();
};

class ValueScanPrototype : public POProt {
    const Comparison cmp;
    counted_ptr<MemoryTupleSequence> value;
public:
    ValueScanPrototype(PhysicalModel * model, const POProtIn & _left, const TupleRef & _right, const Comparison& _cmp);

    virtual void evaluateCost(CostModel* model);
    virtual phop::IOperator * compile();
};

/*
 * This operation should decide how to join its operands in the best way possible
 */

class MagicJoinPrototype : public BinaryOpPrototype {
    pe::Path path;
protected:
    virtual IElementProducer* __toXML(IElementProducer* ) const;
public:
    MagicJoinPrototype(PhysicalModel * model, const POProtIn & _left, const POProtIn & _right, const pe::Path& _path);

    virtual void evaluateCost(CostModel* model);
    virtual phop::IOperator * compile();
};


class ValidatePathPrototype : public POProt {
    DataRoot dataRoot;
    pe::Path path;
protected:
    virtual IElementProducer* __toXML(IElementProducer* ) const;
public:
    ValidatePathPrototype(PhysicalModel * model, const POProtIn & _tuple);

    virtual void evaluateCost(CostModel* model);
    virtual phop::IOperator * compile();
};


#endif /* _OPERATIONS_H */
