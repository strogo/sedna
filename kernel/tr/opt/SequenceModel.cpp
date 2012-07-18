#include "SequenceModel.h"

#include "tr/opt/graphs/DataGraphs.h"
#include "tr/models/XmlConstructor.h"

using namespace phop;

std::stack<GraphExecutionBlock * > GraphExecutionBlock::blockBuildingStack;

OPINFO_DEF(ReduceToItemOperator)

void GraphExecutionBlock::prepare(const opt::DataGraphIndex* dgi)
{
    U_ASSERT(sourceStack.empty());
    operatorMap.clear();

    if (dgi != NULL) {
        TupleIdMap realMap;

        for (opt::DataNodeList::const_iterator it = dgi->out.begin(); it != dgi->out.end(); ++it)
        {
            opt::DataNode * node = *it;
            if (node->varTupleId != opt::invalidTupleId)
            {
                realMap[node->varTupleId] = resultMap[node->absoluteIndex];
            };
        };

        resultMap = realMap;
    };
}

void BinaryTupleOperator::reset()
{
    ITupleOperator::reset();

    left.op = dynamic_cast<ITupleOperator *>(block->body[leftIdx]);
    right.op = dynamic_cast<ITupleOperator *>(block->body[rightIdx]);

    left.op->reset();
    right.op->reset();
}

void UnaryTupleOperator::reset()
{
    phop::ITupleOperator::reset();
    in.op = dynamic_cast<ITupleOperator *>(block->body[inIdx]);
    in.op->reset();
}

void ItemOperator::reset()
{
    phop::IValueOperator::reset();
    in = dynamic_cast<IValueOperator *>(block->body[inIdx]);
    in->reset();
}

IOperator::IOperator(OPINFO_T _opinfo) : opinfo(_opinfo), block(NULL)
{
    block = GraphExecutionBlock::current();
    block->body.push_back(this);
    block->operatorMap.insert(OperatorMap::value_type(this, block->body.size()-1));
}

IOperator::~IOperator()
{
    //
}

ReduceToItemOperator::ReduceToItemOperator(const phop::TupleIn& op, bool _nested)
    : IValueOperator(OPINFO_REF), in(op), nested(_nested)
{
    inIdx = block->operatorMap.at(op.op);
}

void ReduceToItemOperator::do_next()
{
    in->next();
    
    if (in->get().is_eos()) {
        seteos();
        return;
    };

    push(in.get());
}

void ReduceToItemOperator::reset()
{
    phop::IValueOperator::reset();
    in.op = dynamic_cast<ITupleOperator *>(block->body[inIdx]);
    in->reset();
}

XmlConstructor & IOperator::toXML(XmlConstructor & element) const
{
    element.openElement(PHOPQNAME(info()->name));
    __toXML(element);
    element.closeElement();
    return element;
}

BinaryTupleOperator::BinaryTupleOperator(OPINFO_T _opinfo, unsigned _size, const MappedTupleIn & _left, const MappedTupleIn & _right)
    : ITupleOperator(_opinfo, _size), left(_left), right(_right)
{
    leftIdx = block->operatorMap.at(_left.op);
    rightIdx = block->operatorMap.at(_right.op);
};

UnaryTupleOperator::UnaryTupleOperator(const phop::operation_info_t* _opinfo, unsigned int _size, const phop::MappedTupleIn& _in)
    : ITupleOperator(_opinfo, _size), in(_in)
{
    inIdx = block->operatorMap.at(_in.op);
}

ItemOperator::ItemOperator(const phop::operation_info_t* _opinfo, IValueOperator* _in)
    : IValueOperator(_opinfo), in(_in)
{
    inIdx = block->operatorMap.at(_in);
}



XmlConstructor & BinaryTupleOperator::__toXML(XmlConstructor & element ) const
{
    left.op->toXML(element);
    right.op->toXML(element);
    return element;
}

XmlConstructor & UnaryTupleOperator::__toXML(XmlConstructor & element ) const
{
    in.op->toXML(element);
    return element;
}

XmlConstructor & ReduceToItemOperator::__toXML(XmlConstructor & element) const
{
    return element;
}

XmlConstructor & ReduceToItemOperator::toXML(XmlConstructor & element) const
{
    if (!nested) {
//        element = element->addElement(PHOPQNAME(info()->name));
        return in.op->toXML(element);
//        element->close();
    }
    return element;
}

XmlConstructor & ItemOperator::__toXML(XmlConstructor & element ) const
{
    in->toXML(element);
    return element;
}