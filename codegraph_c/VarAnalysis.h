#pragma once

#include <memory>
#include <vector>

namespace codegraph
{

class AstNode;

class VarAnalysis
{
public:
	VarAnalysis(const std::shared_ptr<AstNode>& func);

	void GetNodes(std::vector<std::shared_ptr<AstNode>>& variables,
		std::vector<std::shared_ptr<AstNode>>& statements) const;

private:
	std::shared_ptr<AstNode> m_func = nullptr;

}; // VarAnalysis

}