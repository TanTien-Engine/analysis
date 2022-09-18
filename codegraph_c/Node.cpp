#include "Node.h"

#include <cslang/Declaration.h>
#include <cslang/GenCode.h>

#include <sstream>
#include <iostream>

namespace codegraph
{

Node::Node(const std::shared_ptr<cslang::Tokenizer>& tokenizer,
	       const std::shared_ptr<cslang::Node>& node)
	: m_tokenizer(tokenizer)
	, m_node(node)
{
}

void Node::Print() const
{
    auto tu_node = std::dynamic_pointer_cast<cslang::ast::TranslationUnitNode>(m_node);
    if (tu_node) 
    {
        std::stringstream ss;
        cslang::GenTranslationUnit(ss, tu_node);
        std::cout << ss.str() << std::endl;
    }
}

const char* Node::GetName() const
{
	return "zz";
}

}