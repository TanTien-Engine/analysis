#pragma once

#include <memory>

namespace cslang { class Tokenizer; }
namespace cslang::ast { struct FunctionNode; struct StatementNode; }

namespace codegraph
{

class AstNode;
class BasicBlock;
struct FuncContext;

class AstToCfg
{
public:
	static std::shared_ptr<BasicBlock> Gen(const std::shared_ptr<AstNode>& node);

	static std::shared_ptr<BasicBlock> 
		GenFunc(const std::shared_ptr<cslang::Tokenizer>& tokenizer, const std::shared_ptr<cslang::ast::FunctionNode>& func);

	static std::shared_ptr<BasicBlock> 
		GenStat(const std::shared_ptr<cslang::Tokenizer>& tokenizer, const std::shared_ptr<cslang::ast::StatementNode>& stmt, FuncContext& ctx);

}; // AstToCfg

}