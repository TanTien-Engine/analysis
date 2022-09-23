#pragma once

#include <memory>
#include <string>

namespace codegraph
{

class Node;

void NodeRename(const Node& node, const std::string& name);

}