#include "Node.h"

namespace loggraph
{

std::set<std::string> Node::m_strings;

Node::Node(const std::string& type, const std::string& name)
	: m_type(type)
	, m_name(name)
{
}

void Node::AddChild(const std::shared_ptr<Node>& c)
{
	m_children.push_back(c);
}

void Node::AddData(int i)
{
	Variant var;
	var.type = VarType::Integer;
	var.i = i;
	m_data.push_back(var);
}

void Node::AddData(double d)
{
	Variant var;
	var.type = VarType::Double;
	var.d = d;
	m_data.push_back(var);
}

void Node::AddData(const std::string& s)
{
	Variant var;
	var.type = VarType::String;

	auto itr = m_strings.find(s);
	if (itr == m_strings.end()) {
		auto itr = m_strings.insert(s);
		var.obj = itr.first->data();
	} else {
		var.obj = itr->data();
	}

	m_data.push_back(var);
}

}