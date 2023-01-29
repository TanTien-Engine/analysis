#include "Node.h"

namespace loggraph
{

Node::Node(const std::string& type, const std::string& name)
	: m_type(type)
	, m_name(name)
{
}

Node::~Node()
{
	// fixme: move to Variant
	for (auto var : m_data)
	{
		if (var.type == VarType::Group)
		{
			auto children = reinterpret_cast<const VarGroup*>(var.obj);
			delete children;
		}
	}
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