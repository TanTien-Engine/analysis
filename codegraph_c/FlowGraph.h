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

	auto GetRoot() const { return m_root; }

private:
	void Build(const std::shared_ptr<Node>& node);

	std::shared_ptr<BasicBlock> BuildFunc(const std::shared_ptr<cslang::Tokenizer>& tokenizer,
		const std::shared_ptr<cslang::ast::FunctionNode>& func) const;
	std::shared_ptr<BasicBlock> BuildStatement(const std::shared_ptr<cslang::Tokenizer>& tokenizer, 
		const std::shared_ptr<cslang::ast::StatementNode>& stmt) const;

private:
	std::shared_ptr<BasicBlock> m_root = nullptr;

}; // FlowGraph

}