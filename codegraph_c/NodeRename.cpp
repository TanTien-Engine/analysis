#include "AstNodeImpl.h"
#include "AstNode.h"
#include "NodeTraversal.h"

#include <cslang/Declaration.h>

namespace
{

class RenameVisitor : public codegraph::Visitor
{
public:
	RenameVisitor(const char* from, const char* to)
		: m_from(from), m_to(to)
	{
	}

	virtual void VisitExpression(const std::shared_ptr<cslang::ast::ExpressionNode>& expr) override
	{
		switch (expr->op)
		{
		case cslang::ast::OP_CALL:
			if (expr->kids[0]->op == cslang::ast::OP_ID) {
				if (strcmp((char*)expr->kids[0]->val.p, m_from) == 0) {
					expr->kids[0]->val.p = const_cast<char*>(m_to);
				}
			}

			break;
		}
	}

private:
	const char *m_from = nullptr, *m_to = nullptr;

}; // RenameVisitor

}

namespace codegraph
{

void NodeRename(const AstNode& _node, const std::string& name)
{
	auto node = _node.GetNode();
	switch (node->kind)
	{
	case cslang::NK_Function:
	{
		auto func = std::static_pointer_cast<cslang::ast::FunctionNode>(node);
		auto f = func->fdec->dec->id;
		auto t = _node.GetTokenizer()->AddString(name);

		RenameVisitor visitor(f, t);
		Traversal(_node.GetRoot(), visitor);

		func->fdec->dec->id = const_cast<char*>(t);
	}
		break;
	}
}

}