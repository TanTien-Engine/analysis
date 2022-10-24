#include "NodeExpand.h"
#include "AstNode.h"
#include "NodeTraversal.h"

#include <cslang/Declaration.h>

namespace
{

class ExpandVisitor : public codegraph::Visitor
{
public:
	ExpandVisitor(const std::shared_ptr<cslang::Tokenizer>& tokenizer, 
		const std::shared_ptr<cslang::Node>& root)
		: m_tokenizer(tokenizer)
		, m_root(root)
	{
	}

	virtual void VisitFunction(const std::shared_ptr<cslang::ast::FunctionNode>& func) override 
	{
		m_children.push_back(std::make_shared<codegraph::AstNode>(m_tokenizer, func, m_root));
	}

	auto& GetChildren() const { return m_children; }

private:
	std::shared_ptr<cslang::Tokenizer> m_tokenizer = nullptr;
	std::shared_ptr<cslang::Node> m_root = nullptr;

	std::vector<std::shared_ptr<codegraph::AstNode>> m_children;

}; // ExpandVisitor

}

namespace codegraph
{

std::vector<std::shared_ptr<AstNode>> NodeExpand(const AstNode& _node)
{
    auto node = _node.GetNode();
    if (!node) {
		return {};
    }

	auto tokenizer = _node.GetTokenizer();
    auto root = _node.GetRoot();

	ExpandVisitor visitor(tokenizer, root);
	Traversal(node, visitor);

	return visitor.GetChildren();
}

}