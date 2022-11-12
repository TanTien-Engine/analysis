#include "BBlockTools.h"
#include "BasicBlock.h"

#include <cslang/Statement.h>

namespace codegraph
{

bool BBlockTools::IsLoop(const std::shared_ptr<BasicBlock>& bb)
{
    return CheckType(bb, { cslang::NK_ForStatement, cslang::NK_WhileStatement, cslang::NK_DoStatement });
}

bool BBlockTools::IsGoto(const std::shared_ptr<BasicBlock>& bb)
{
    return CheckType(bb, { cslang::NK_GotoStatement });
}

bool BBlockTools::IsLabel(const std::shared_ptr<BasicBlock>& bb)
{
    return CheckType(bb, { cslang::NK_LabelStatement });
}

bool BBlockTools::CheckType(const std::shared_ptr<BasicBlock>& bb, const std::vector<int>& types)
{
    auto& nodes = bb->GetNodes();
    if (nodes.empty()) {
        return false;
    }

    bool ret = false;

    auto stmt = std::static_pointer_cast<cslang::ast::StatementNode>(nodes.back());
    for (auto type : types) {
        if (type == stmt->kind) {
            ret = true;
            break;
        }
    }

    return ret;
}

}