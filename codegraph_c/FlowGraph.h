#pragma once

#include <memory>
#include <vector>

namespace cslang { class Tokenizer; }
namespace cslang::ast { struct FunctionNode; struct StatementNode; }

namespace codegraph
{

class Node;
class BasicBlock;

class FlowGraph
{
public:
	FlowGraph(const std::shared_ptr<Node>& node);

	auto& GetFuncNodes() const { return m_func_nodes; }

private:
	void Build(const std::shared_ptr<Node>& node);

	std::shared_ptr<BasicBlock> BuildFunc(const std::shared_ptr<cslang::Tokenizer>& tokenizer,
		const std::shared_ptr<cslang::ast::FunctionNode>& func) const;
	std::shared_ptr<BasicBlock> BuildStatement(const std::shared_ptr<cslang::Tokenizer>& tokenizer, 
		const std::shared_ptr<cslang::ast::StatementNode>& stmt) const;

private:
	std::vector<std::shared_ptr<BasicBlock>> m_func_nodes;

}; // FlowGraph

}