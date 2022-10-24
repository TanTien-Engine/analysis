#include "AstNode.h"
#include "AstNodeImpl.h"

namespace codegraph
{

AstNode::AstNode(const std::shared_ptr<cslang::Tokenizer>& tokenizer,
	       const std::shared_ptr<cslang::Node>& node,
	       const std::shared_ptr<cslang::Node>& root)
	: m_tokenizer(tokenizer)
	, m_node(node)
	, m_root(root)
{
}

void AstNode::Print() const
{
	NodePrint(*this);
}

std::string AstNode::GetName() const
{
	return NodeName(*this);
}

void AstNode::SetName(const std::string& name)
{
	NodeRename(*this, name);
}

std::vector<std::shared_ptr<AstNode>> AstNode::GetChildren() const
{
	return NodeExpand(*this);
}

}