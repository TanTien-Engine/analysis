#pragma once

#include <memory>
#include <vector>

namespace codegraph
{

class Node;

class VarAnalysis
{
public:
	VarAnalysis(const std::shared_ptr<Node>& func);

	void GetNodes(std::vector<std::shared_ptr<Node>>& variables, 
		std::vector<std::shared_ptr<Node>>& statements) const;

private:
	std::shared_ptr<Node> m_func = nullptr;

}; // VarAnalysis

}