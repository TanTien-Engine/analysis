#include "BasicBlock.h"

#include <cslang/AST.h>
#include <cslang/Declaration.h>
#include <cslang/GenCode.h>

#include <sstream>
#include <iostream>

namespace codegraph
{

BasicBlock::BasicBlock(const std::shared_ptr<cslang::Tokenizer>& tokenizer, const std::string& name)
	: m_tokenizer(tokenizer)
	, m_name(name)
{
}

void BasicBlock::Print() const
{
	std::stringstream ss;

	for (auto& node : m_nodes)
	{
		if (node->kind == cslang::NK_Function)
		{
			auto func = std::static_pointer_cast<cslang::ast::FunctionNode>(node);
			cslang::GenFunction(ss, func);
		}
		else
		{
			auto stmt = std::static_pointer_cast<cslang::ast::StatementNode>(node);
			cslang::GenStatement(ss, stmt, 0);
		}
	}

	std::cout << ss.str() << std::endl;
}

}