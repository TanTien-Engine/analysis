#pragma once

#include <vector>
#include <memory>
#include <string>

namespace codegraph
{

class Node;

class NodeExpand
{
public:
	static std::string GetName(const Node& node);
	static std::vector<std::shared_ptr<Node>> GetChildren(const Node& node);

}; // NodeExpand

}