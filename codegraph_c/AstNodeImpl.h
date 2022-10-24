#pragma once

#include <vector>
#include <memory>
#include <string>

namespace codegraph
{

class AstNode;

std::string NodeName(const AstNode& node);

std::vector<std::shared_ptr<AstNode>> NodeExpand(const AstNode& node);

void NodePrint(const AstNode& node);

void NodeRename(const AstNode& node, const std::string& name);

}