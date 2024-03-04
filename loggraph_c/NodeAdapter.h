#pragma once

#include <memory>

namespace pm3 { class Polytope; }
namespace graph { class Graph; }

namespace loggraph
{

class Node;

class NodeAdapter
{
public:
	static std::shared_ptr<pm3::Polytope> ToPolytope(const std::shared_ptr<Node>& node);

	static std::shared_ptr<graph::Graph> ToGraph(const std::shared_ptr<Node>& node);

}; // NodeAdapter

}