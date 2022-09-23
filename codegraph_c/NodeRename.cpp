#include "NodeRename.h"
#include "Node.h"

#include <cslang/Declaration.h>

namespace
{

void set_caller_name(const std::shared_ptr<cslang::Node>& node, const char* from, const char* to)
{
	switch (node->kind)
	{
	case cslang::NK_TranslationUnit:
	{
		auto root = std::static_pointer_cast<cslang::ast::TranslationUnitNode>(node);
		auto p = root->extDecls;
		while (p) {
			set_caller_name(p, from, to);
			p = p->next;
		}
	}
		break;
	case cslang::NK_Function:
	{
		auto func = std::static_pointer_cast<cslang::ast::FunctionNode>(node);
		set_caller_name(func->stmt, from, to);
	}
		break;
	case cslang::NK_ReturnStatement:
	{
		auto ret = std::static_pointer_cast<cslang::ast::ReturnStmtNode>(node);
		set_caller_name(ret->expr, from, to);
	}
		break;

	case cslang::NK_Expression:
	{
		auto expr = std::static_pointer_cast<cslang::ast::ExpressionNode>(node);
		switch (expr->op)
		{
		case cslang::ast::OP_CALL:
			if (expr->kids[0]->op == cslang::ast::OP_ID) {
				if (strcmp((char*)expr->kids[0]->val.p, from) == 0) {
					expr->kids[0]->val.p = const_cast<char*>(to);
				}
			}
			
			break;
		}
	}
		break;

	case cslang::NK_CompoundStatement:
	{
		auto p = std::static_pointer_cast<cslang::ast::CompoundStmtNode>(node)->stmts;
		while (p != NULL) {
			set_caller_name(p, from, to);
			p = p->next;
		}
	}
		break;
	}
}

}

namespace codegraph
{

void NodeRename(const Node& _node, const std::string& name)
{
	auto node = _node.GetNode();
	switch (node->kind)
	{
	case cslang::NK_Function:
	{
		auto func = std::static_pointer_cast<cslang::ast::FunctionNode>(node);
		auto f = func->fdec->dec->id;
		auto t = _node.GetTokenizer()->AddString(name);
		set_caller_name(_node.GetRoot(), f, t);
		func->fdec->dec->id = const_cast<char*>(t);
	}
		break;
	}
}

}