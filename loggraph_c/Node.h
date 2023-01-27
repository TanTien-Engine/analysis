#pragma once

#include "Variant.h"

#include <string>
#include <vector>
#include <memory>
#include <set>

namespace loggraph
{

class Node
{
public:
	Node() {}
	Node(const std::string& type, const std::string& name);

	auto& GetType() const { return m_type; }
	auto& GetName() const { return m_name; }

	auto& GetAllChildren() const { return m_children; }
	auto& GetAllData() const { return m_data; }

	void AddChild(const std::shared_ptr<Node>& c);

	void AddData(int i);
	void AddData(double d);
	void AddData(const std::string& s);

private:
	std::string m_type;
	std::string m_name;

	std::vector<std::shared_ptr<Node>> m_children;

	std::vector<Variant> m_data;

	static std::set<std::string> m_strings;

}; // Node

}
