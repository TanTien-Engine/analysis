#pragma once

#include <memory>
#include <string>
#include <vector>

namespace cslang { class Tokenizer; struct Node; }
namespace cslang::ast { struct StatementNode; struct CompoundStmtNode; struct SwitchStmtNode; }

namespace codegraph
{

class BasicBlock;
struct FuncContext;

class BBlockBuilder
{
public:
	static void Connect(const std::shared_ptr<codegraph::BasicBlock>& list0,
		const std::shared_ptr<codegraph::BasicBlock>& list1);

	static std::shared_ptr<codegraph::BasicBlock>
		Merge(const std::shared_ptr<cslang::Tokenizer>& tokenizer, std::vector<std::shared_ptr<cslang::Node>>& merged);

	static std::shared_ptr<codegraph::BasicBlock> Split(const std::shared_ptr<cslang::Tokenizer>& tokenizer, 
		const std::shared_ptr<cslang::ast::CompoundStmtNode>& comp, FuncContext& ctx);
	static std::vector<std::shared_ptr<codegraph::BasicBlock>>
		Split(const std::shared_ptr<cslang::Tokenizer>& tokenizer, const std::shared_ptr<cslang::ast::SwitchStmtNode>& switch_stmt);

	static std::string GetName(const std::shared_ptr<cslang::ast::StatementNode>& node);

}; // BBlockBuilder

}