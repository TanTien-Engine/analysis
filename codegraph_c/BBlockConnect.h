#pragma once

#include <memory>
#include <vector>

namespace codegraph
{

class BasicBlock;

class BBlockConnect
{
public:
	static void Connect(const std::shared_ptr<BasicBlock>& root);

private:
	static std::pair<std::shared_ptr<BasicBlock>, std::shared_ptr<BasicBlock>> 
		BuildGraph(const std::shared_ptr<BasicBlock>& entry);

	static std::vector<std::shared_ptr<BasicBlock>> BuildSubgraph(const std::shared_ptr<BasicBlock>& prev,
		const std::vector<std::shared_ptr<BasicBlock>>& children);

	static void ConnectNTo1(const std::vector<std::shared_ptr<BasicBlock>>& prev_nodes,
		const std::shared_ptr<BasicBlock>& next_node, const std::shared_ptr<BasicBlock>& prev_node);

}; // BBlockConnect

}