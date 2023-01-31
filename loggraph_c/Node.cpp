#include "Node.h"

namespace loggraph
{

Node::Node(const std::string& type, const std::string& name)
	: m_type(type)
	, m_name(name)
{
}

void Node::AddChild(const std::shared_ptr<Node>& c)
{
	m_children.push_back(c);
}

void Node::AddData(const Variant& var)
{
	m_data.push_back(var);
}

}