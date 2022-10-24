#pragma once

#include <vector>
#include <memory>

namespace codegraph
{

class AstNode;

std::vector<std::shared_ptr<AstNode>> NodeExpand(const AstNode& node);

}