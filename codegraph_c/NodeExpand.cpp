#include "NodeExpand.h"
#include "Node.h"
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
		m_children.push_back(std::make_shared<codegraph::Node>(m_tokenizer, func, m_root));
	}

	auto& GetChildren() const { return m_children; }

private:
	std::shared_ptr<cslang::Tokenizer> m_tokenizer = nullptr;
	std::shared_ptr<cslang::Node> m_root = nullptr;

	std::vector<std::shared_ptr<codegraph::Node>> m_children;

}; // ExpandVisitor

std::string to_string(const std::shared_ptr<cslang::ast::ExpressionNode>& expr)
{
	std::string str;

	switch (expr->op)
	{
	case cslang::ast::OP_ID:
		str =(char*)(expr->val.p);
		break;
	}

	return str;
}

}

namespace codegraph
{

std::string NodeExpand::GetName(const Node& _node)
{
	std::string name = "unknown";

	auto node = _node.GetNode();
	if (!node) {
		return name;
	}

    switch (node->kind)
    {
    case cslang::NK_TranslationUnit:
        name = "root";
        break;
    case cslang::NK_Function:
        name = std::static_pointer_cast<cslang::ast::FunctionNode>(node)->fdec->dec->id;
        break;
	case cslang::NK_Expression:
	{
		auto expr = std::static_pointer_cast<cslang::ast::ExpressionNode>(node);
		switch (expr->op)
		{
		case cslang::ast::OP_CALL:
			name = to_string(expr->kids[0]);
			break;
		}
	}
		break;
    }

    return name;
}

std::vector<std::shared_ptr<Node>> NodeExpand::GetChildren(const Node& _node)
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