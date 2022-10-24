#include "VarAnalysis.h"
#include "NodeTraversal.h"
#include "AstNode.h"

#include <cslang/Declaration.h>

namespace
{

class VarAndStatVisitor : public codegraph::Visitor
{
public:
	VarAndStatVisitor(const std::shared_ptr<cslang::Tokenizer>& tokenizer,
		const std::shared_ptr<cslang::Node>& root)
		: m_tokenizer(tokenizer)
		, m_root(root)
	{
	}

	virtual void VisitStatement(const std::shared_ptr<cslang::ast::StatementNode>& stat) override
	{
		switch (stat->kind)
		{
		case cslang::NK_Declaration:
			m_variables.push_back(std::make_shared<codegraph::AstNode>(m_tokenizer, stat, m_root));
			break;
		case cslang::NK_ForStatement:
			m_statements.push_back(std::make_shared<codegraph::AstNode>(m_tokenizer, stat, m_root));
			break;
		}
	}

	auto& GetVariables() const { return m_variables; }
	auto& GetStatements() const { return m_statements; }

private:
	std::shared_ptr<cslang::Tokenizer> m_tokenizer = nullptr;
	std::shared_ptr<cslang::Node> m_root = nullptr;

	std::vector<std::shared_ptr<codegraph::AstNode>> m_variables;
	std::vector<std::shared_ptr<codegraph::AstNode>> m_statements;

}; // VarAndStatVisitor

}

namespace codegraph
{

VarAnalysis::VarAnalysis(const std::shared_ptr<AstNode>& func)
	: m_func(func)
{
}

void VarAnalysis::GetNodes(std::vector<std::shared_ptr<AstNode>>& variables,
	                       std::vector<std::shared_ptr<AstNode>>& statements) const
{
    auto node = m_func->GetNode();
    if (!node) {
		return;
    }

	auto tokenizer = m_func->GetTokenizer();

	VarAndStatVisitor visitor(tokenizer, node);
	Traversal(node, visitor);

	variables = visitor.GetVariables();
	statements = visitor.GetStatements();
}

}