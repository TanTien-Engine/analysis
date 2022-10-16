#pragma once

#include <memory>
#include <vector>

namespace codegraph
{

class Node;
class BasicBlock;

class FlowGraph
{
public:
	FlowGraph(const std::shared_ptr<Node>& node);

	auto& GetNodes() const { return m_nodes; }

private:
	void Build(const std::shared_ptr<Node>& node);

private:
	//std::shared_ptr<BasicBlock> m_entry = nullptr;

	std::vector<std::shared_ptr<BasicBlock>> m_nodes;

}; // FlowGraph

}