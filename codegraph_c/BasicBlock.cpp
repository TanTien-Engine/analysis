#include "BasicBlock.h"

#include <cslang/AST.h>
#include <cslang/Declaration.h>
#include <cslang/GenCode.h>

#include <sstream>
#include <iostream>

namespace codegraph
{

BasicBlock::BasicBlock(const std::shared_ptr<cslang::Tokenizer>& tokenizer,
	                   const std::shared_ptr<cslang::Node>& node, const std::string& name)
	: m_tokenizer(tokenizer)
	, m_node(node)
	, m_name(name)
{
}

void BasicBlock::Print() const
{
	std::stringstream ss;

	switch (m_node->kind)
	{
	case cslang::NK_Function:
	{
		auto func = std::static_pointer_cast<cslang::ast::FunctionNode>(m_node);
		cslang::GenFunction(ss, func);
	}
		break;
	}

	std::cout << ss.str() << std::endl;
}

}