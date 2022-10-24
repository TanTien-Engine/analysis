#include "NodeName.h"
#include "AstNode.h"

#include <cslang/Declaration.h>

namespace
{

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

std::string NodeName(const AstNode& _node)
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
		case cslang::ast::OP_ADD_ASSIGN:
			name = to_string(expr->kids[1]);
			break;
		case cslang::ast::OP_ID:
			name = to_string(expr);
			break;
		default:
			assert(0);
		}
	}
		break;
	case cslang::NK_ForStatement:
		name = "for";
		break;
	case cslang::NK_Declaration:
	{
		auto decl = std::static_pointer_cast<cslang::ast::DeclarationNode>(node);
		auto init_dec = std::static_pointer_cast<cslang::ast::InitDeclaratorNode>(decl->initDecs);
		name = init_dec->dec->id;
	}
		break;
	default:
		assert(0);
    }

	assert(name != "unknown");

    return name;
}

}