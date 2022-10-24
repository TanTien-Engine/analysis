#pragma once

#include <memory>
#include <string>

namespace codegraph
{

class AstNode;

void NodeRename(const AstNode& node, const std::string& name);

}