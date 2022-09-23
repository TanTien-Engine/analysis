#include "Node.h"
#include "NodeRename.h"
#include "NodeExpand.h"
#include "NodePrint.h"

namespace codegraph
{

Node::Node(const std::shared_ptr<cslang::Tokenizer>& tokenizer,
	       const std::shared_ptr<cslang::Node>& node,
	       const std::shared_ptr<cslang::Node>& root)
	: m_tokenizer(tokenizer)
	, m_node(node)
	, m_root(root)
{
}

void Node::Print() const
{
	NodePrint(*this);
}

std::string Node::GetName() const
{
	return NodeExpand::GetName(*this);
}

void Node::SetName(const std::string& name)
{
	NodeRename(*this, name);
}

std::vector<std::shared_ptr<Node>> Node::GetChildren() const
{
	return NodeExpand::GetChildren(*this);
}

}