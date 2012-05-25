#include "tr/opt/OptTypes.h"
#include "tr/opt/algebra/IndependentPlan.h"
#include "tr/opt/algebra/DataGraph.h"
#include "tr/opt/algebra/Predicates.h"

using namespace opt;
using namespace rqp;

struct DataReductionBlock {
    RPEdge in;
    OperationList out;
    TupleMapping externalBindings;
    
    OperationList members;

    DataGraph* buildBlock();
};

class DataGraphReduction {
    std::vector<DataReductionBlock *> blocks;
    std::map<RPBase *, DataReductionBlock *> opBlockMap;
    std::stack<DataReductionBlock *> blockStack;

    DataReductionBlock * newBlock()
    {
        DataReductionBlock * result = new DataReductionBlock();
        blocks.push_back(result);
        blockStack.push(result);
        return result;
    }

    void addOp(RPBase * op)
    {
        U_ASSERT(blockStack.top() != NULL);
        blockStack.top()->members.push_back(op);
        opBlockMap[op] = blockStack.top();
    };
    
public:
    void collectBlocks(RPBase * parent, RPBase * op);
    void execute(RPBase * op);
};

typedef std::map<RPBase *, DataNode *> ResultMap;

struct Generator {
    ResultMap map;
    DataGraph * dg;

    DataNode * get(RPBase * op) {
        DataNode * x = NULL;

        if (map.find(op) == map.end()) {
            x = dg->owner->createFreeNode(dg);
            map.insert(ResultMap::value_type(op, x));
        } else {
            x = map.at(op);
        };

        return x;
    };
};

static
std::string * varName(rqp::VarIn * op) {
    return new std::string(op->getContext()->getVarDef(op->getTuple())->getVarLabel());
};

DataGraph * DataReductionBlock::buildBlock()
{
    DataGraphMaster * dgm = PlanContext::current->dgm();
    Generator context;
    context.dg = dgm->createGraph();
    DataNode * node;

    for (OperationList::const_iterator it = members.begin(); it != members.end(); ++it) {
        RPBase * op = *it;

        switch (op->info()->opType) {
          case rqp::Select::opid : {
            U_ASSERT(dynamic_cast<rqp::Select *>(op) != NULL);
            rqp::Select * sop = static_cast<rqp::Select *>(op);

            node = context.get(op);
            node->type = DataNode::dnAlias;
            node->source = context.get(sop->getList());

            externalBindings[sop->getContextTuple()] = node->source->varIndex;
          } break;
          case rqp::Const::opid :
            U_ASSERT(dynamic_cast<rqp::Const *>(op) != NULL);
            
            node = context.get(op);
            node->type = DataNode::dnConst;
            node->sequence = static_cast<rqp::Const *>(op)->getSequence();
            
            break;
          case rqp::VarIn::opid : {
            U_ASSERT(dynamic_cast<rqp::VarIn *>(op) != NULL);
            rqp::VarIn * vop = static_cast<rqp::VarIn *>(op);
            
            node = context.get(op);
            node->type = DataNode::dnFreeNode; // TODO : Fix this stuff
            node->varName = varName(vop);

            externalBindings[vop->getTuple()] = node->varIndex;
            
          } break;
          case rqp::XPathStep::opid : {
            U_ASSERT(dynamic_cast<rqp::XPathStep *>(op) != NULL);
            
            rqp::XPathStep * xop = static_cast<rqp::XPathStep *>(op);
            node = context.get(op);

            dgm->createPath(context.dg,
                node->varIndex,
                context.get(xop->getList())->varIndex,
                xop->getStep());

          } break;
          case rqp::ComparisonExpression::opid : {
            U_ASSERT(dynamic_cast<rqp::ComparisonExpression *>(op) != NULL);

            rqp::ComparisonExpression * cop = static_cast<rqp::ComparisonExpression *>(op);
            
            node = context.get(op);
            node->type = DataNode::dnConst;

            node->sequence = new MemoryTupleSequence();
            node->sequence->push_back(tuple_cell::atomic(true));

            dgm->createComparison(context.dg,
                context.get(cop->getLeft())->varIndex,
                context.get(cop->getRight())->varIndex,
                cop->getOp());

          } break;
          default:
            U_ASSERT(false);
          break;
        };
    };

    return context.dg;
}


void DataGraphReduction::collectBlocks(RPBase * parent, RPBase * op)
{
    bool blockOwner = false;

    if ((op->info()->flags & rqp::oBlockBuilder) > 0) {
        blockOwner = blockStack.top() == NULL;

        if (blockOwner) {
            newBlock();
            blockStack.top()->in = RPEdge(parent, op);
        };

        addOp(op);
    } else if (blockStack.top() != NULL) {
        blockStack.top()->out.push_back(op);
        blockStack.push(NULL);
        blockOwner = true;
    };

    OperationList children;
    op->getChildren(children);

    for (OperationList::const_iterator it = children.begin(); it != children.end(); ++it) {
        if (*it != NULL) {
            collectBlocks(op, *it);
        }
    };

    if (blockOwner) {
        blockStack.pop();
    };
};


void DataGraphReduction::execute(RPBase* op)
{
    blockStack.push(NULL);
    collectBlocks(NULL, op);

    for (std::vector<DataReductionBlock *>::const_iterator it = blocks.begin(); it != blocks.end(); ++it) {
        DataGraph * dg = (*it)->buildBlock();
        DataGraphOperation * op = new DataGraphOperation(dg, (*it)->out, (*it)->externalBindings);
        op->getContext()->replaceOperation((*it)->in.second, op);
    };
}



RPBase* selectDataGraphs(RPBase* op)
{
    DataGraphReduction().execute(op);
    return op;
};
