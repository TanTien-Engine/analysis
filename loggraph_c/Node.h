#pragma once

#include <string>
#include <vector>
#include <memory>

namespace loggraph
{

class Node
{
public:
	std::string type;
	std::string name;

	std::vector<std::shared_ptr<Node>> children;

	std::vector<int> items;
	std::vector<std::string> strings;

}; // Node

}
