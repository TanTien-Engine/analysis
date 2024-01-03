#pragma once

#include <memory>

namespace pm3 { class Polytope; }

namespace loggraph
{

class Node;

class NodeAdapter
{
public:
	static std::shared_ptr<pm3::Polytope> ToPolytope(const std::shared_ptr<Node>& node);

}; // NodeAdapter

}