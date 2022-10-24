#include "NodeTraversal.h"

#include <cslang/AST.h>
#include <cslang/Declaration.h>

namespace
{

void traversal_variant(const std::shared_ptr<cslang::ast::DeclarationNode>& decl, codegraph::Visitor& v)
{
	v.VisitVariant(decl);

	auto init_dec = std::static_pointer_cast<cslang::ast::InitDeclaratorNode>(decl->initDecs);
	auto name = init_dec->dec->id;
	
	//printf("++ var %s\n", name);
}

void traversal_expression(const std::shared_ptr<cslang::ast::ExpressionNode>& expr, codegraph::Visitor& v)
{
	v.VisitExpression(expr);

	if (expr == NULL) {
		return;
	}

    char *opname = NULL;
	if (expr->op >= cslang::ast::OP_COMMA && expr->op <= cslang::ast::OP_POSTDEC) {
		opname = cslang::ast::ExpressionParser::OPNames[expr->op];
	}

	switch (expr->op)
	{
	case cslang::ast::OP_COMMA:
		traversal_expression(expr->kids[0], v);
		traversal_expression(expr->kids[1], v);
        break;
	case cslang::ast::OP_ASSIGN:
	case cslang::ast::OP_BITOR_ASSIGN:
	case cslang::ast::OP_BITXOR_ASSIGN:
	case cslang::ast::OP_BITAND_ASSIGN:
	case cslang::ast::OP_LSHIFT_ASSIGN:
	case cslang::ast::OP_RSHIFT_ASSIGN:
	case cslang::ast::OP_ADD_ASSIGN:
	case cslang::ast::OP_SUB_ASSIGN:
	case cslang::ast::OP_MUL_ASSIGN:
	case cslang::ast::OP_DIV_ASSIGN:
	case cslang::ast::OP_MOD_ASSIGN:
        traversal_expression(expr->kids[0], v);
        traversal_expression(expr->kids[1], v);
        break;
	case cslang::ast::OP_OR:
	case cslang::ast::OP_AND:
	case cslang::ast::OP_BITOR:
	case cslang::ast::OP_BITXOR:
	case cslang::ast::OP_BITAND:
	case cslang::ast::OP_EQUAL:
	case cslang::ast::OP_UNEQUAL:
	case cslang::ast::OP_GREAT:
	case cslang::ast::OP_LESS:
	case cslang::ast::OP_GREAT_EQ:
	case cslang::ast::OP_LESS_EQ:
	case cslang::ast::OP_LSHIFT:
	case cslang::ast::OP_RSHIFT:
	case cslang::ast::OP_ADD:
	case cslang::ast::OP_SUB:
	case cslang::ast::OP_MUL:
	case cslang::ast::OP_DIV:
	case cslang::ast::OP_MOD:
		traversal_expression(expr->kids[0], v);
		traversal_expression(expr->kids[1], v);
		break;
    case cslang::ast::OP_INDEX:
        traversal_expression(expr->kids[0], v);
        traversal_expression(expr->kids[1], v);
        break;

	case cslang::ast::OP_CALL:
    {
		//children.push_back(std::make_shared<codegraph::AstNode>(expr, root));

		//cslang::ast::ExprNodePtr p = expr->kids[1];

  //      traversal_expression(expr->kids[0], children);
  //      while (p)
  //      {
  //          traversal_expression(p, children);
  //          p = std::static_pointer_cast<cslang::ast::ExpressionNode>(p->next);
  //      }
    }
		break;

	case cslang::ast::OP_PREINC:
	case cslang::ast::OP_PREDEC:
	case cslang::ast::OP_POS:
	case cslang::ast::OP_NEG:
	case cslang::ast::OP_NOT:
	case cslang::ast::OP_COMP:
	case cslang::ast::OP_ADDRESS:
	case cslang::ast::OP_DEREF:
		traversal_expression(expr->kids[0], v);
		break;

	case cslang::ast::OP_CAST:
		traversal_expression(expr->kids[0], v);
		break;

	case cslang::ast::OP_SIZEOF:
		if (expr->kids[0]->kind == cslang::NK_Expression) {
			traversal_expression(expr->kids[0], v);
		}
		break;

	case cslang::ast::OP_MEMBER:
	case cslang::ast::OP_PTR_MEMBER:
		traversal_expression(expr->kids[0], v);
		break;

	case cslang::ast::OP_QUESTION:
		traversal_expression(expr->kids[0], v);
		traversal_expression(expr->kids[1]->kids[0], v);
		traversal_expression(expr->kids[1]->kids[1], v);
		break;

	case cslang::ast::OP_POSTINC:
	case cslang::ast::OP_POSTDEC:
		traversal_expression(expr->kids[0], v);
		break;

	case cslang::ast::OP_ID:
		break;

	case cslang::ast::OP_STR:
		//{
		//	auto str = (char*)(expr->val.p);
		//	for (int i = 0, n = strlen(str); i < n; ++i)
		//	{
		//		std::string s;
		//		if (isprint(str[i])) {
		//			s = cslang::StringFormat("%c", str[i]);
		//		} else {
		//			s = cslang::StringFormat("\\x%x", str[i]);
		//		}
		//		output << s;
		//	}

  //          output << "\"";
		//}
		break;

	case cslang::ast::OP_CONST:
		//{
		//	int categ = expr->ty->categ;
  //          std::string s;
		//	if (categ == CHAR || categ == INT || categ == LONG || categ == LONGLONG) {
  //              s = StringFormat("%d", expr->val.i[0]);
		//	} else if (categ == UCHAR || categ == UINT || categ == ULONG || categ == ULONGLONG || categ == POINTER) {
  //              s = StringFormat("%u", expr->val.i[0]);
		//	} else if (categ == FLOAT) {
  //              s = PrintFloatConstant(expr->val.f);
		//	} else {
  //              s = PrintDoubleConstant(expr->val.d);
  //          }
  //          output << s;
		//}
		break;

	default:
		assert(0);
        //output << "ERR";
		break;
	}
}

void traversal_statement(const std::shared_ptr<cslang::ast::StatementNode>& stmt, codegraph::Visitor& v)
{
	v.VisitStatement(stmt);

	switch (stmt->kind)
	{
	case cslang::NK_ExpressionStatement:
		traversal_expression(std::static_pointer_cast<cslang::ast::ExprStmtNode>(stmt)->expr, v);
		break;

	case cslang::NK_LabelStatement:
		traversal_statement(std::static_pointer_cast<cslang::ast::LabelStmtNode>(stmt)->stmt, v);
		break;

	case cslang::NK_CaseStatement:
		traversal_expression(std::static_pointer_cast<cslang::ast::CaseStmtNode>(stmt)->expr, v);
		traversal_statement(std::static_pointer_cast<cslang::ast::CaseStmtNode>(stmt)->stmt, v);
		break;

	case cslang::NK_DefaultStatement:
		traversal_statement(std::static_pointer_cast<cslang::ast::DefaultStmtNode>(stmt)->stmt, v);
		break;

	case cslang::NK_IfStatement:
		traversal_expression(std::static_pointer_cast<cslang::ast::IfStmtNode>(stmt)->expr, v);
		traversal_statement(std::static_pointer_cast<cslang::ast::IfStmtNode>(stmt)->then_stmt, v);
		if (std::static_pointer_cast<cslang::ast::IfStmtNode>(stmt)->else_stmt != NULL) {
			traversal_statement(std::static_pointer_cast<cslang::ast::IfStmtNode>(stmt)->else_stmt, v);
		}
		break;

	case cslang::NK_SwitchStatement:
		traversal_expression(std::static_pointer_cast<cslang::ast::SwitchStmtNode>(stmt)->expr, v);
		traversal_statement(std::static_pointer_cast<cslang::ast::SwitchStmtNode>(stmt)->stmt, v);
		break;

	case cslang::NK_WhileStatement:
		traversal_expression(std::static_pointer_cast<cslang::ast::LoopStmtNode>(stmt)->expr, v);
		traversal_statement(std::static_pointer_cast<cslang::ast::LoopStmtNode>(stmt)->stmt, v);
		break;

	case cslang::NK_DoStatement:
        traversal_statement(std::static_pointer_cast<cslang::ast::LoopStmtNode>(stmt)->stmt, v);
		traversal_expression(std::static_pointer_cast<cslang::ast::LoopStmtNode>(stmt)->expr, v);
		break;

	case cslang::NK_ForStatement:
        traversal_variant(std::static_pointer_cast<cslang::ast::ForStmtNode>(stmt)->init_decl, v);
		traversal_expression(std::static_pointer_cast<cslang::ast::ForStmtNode>(stmt)->expr, v);
		traversal_expression(std::static_pointer_cast<cslang::ast::ForStmtNode>(stmt)->incr_expr, v);
		traversal_statement(std::static_pointer_cast<cslang::ast::ForStmtNode>(stmt)->stmt, v);
		break;

	case cslang::NK_GotoStatement:
		//print(output, "goto %s;", std::static_pointer_cast<GotoStmtNode>(stmt)->id);
		break;

	case cslang::NK_ContinueStatement:
        //output << "continue;";
		break;

	case cslang::NK_BreakStatement:
        //output << "break;";
		break;

	case cslang::NK_ReturnStatement:
		traversal_expression(std::static_pointer_cast<cslang::ast::ReturnStmtNode>(stmt)->expr, v);
		break;

	case cslang::NK_CompoundStatement:
		{
			auto p = std::static_pointer_cast<cslang::ast::CompoundStmtNode>(stmt)->stmts;
			while (p != NULL) {
				traversal_statement(std::static_pointer_cast<cslang::ast::StatementNode>(p), v);
				p = p->next;
			}
			break;
		}

    case cslang::NK_Declaration:
        traversal_variant(std::reinterpret_pointer_cast<cslang::ast::DeclarationNode>(stmt), v);
        break;

	default:
		assert(0);
	}
}

void traversal_function(const std::shared_ptr<cslang::ast::FunctionNode>& func, codegraph::Visitor& v)
{
	v.VisitFunction(func);
	traversal_statement(func->stmt, v);
}

}

namespace codegraph
{

void Traversal(const std::shared_ptr<cslang::Node>& node, Visitor& v)
{
	switch (node->kind)
	{
	case cslang::NK_TranslationUnit:
	{
		auto trans = std::static_pointer_cast<cslang::ast::TranslationUnitNode>(node);
		auto p = trans->extDecls;
		while (p) 
		{
			switch (p->kind)
			{
			case cslang::NK_Declaration:
				traversal_variant(std::static_pointer_cast<cslang::ast::DeclarationNode>(p), v);
				break;
			case cslang::NK_Function:
				traversal_function(std::static_pointer_cast<cslang::ast::FunctionNode>(p), v);
				break;
			}
			p = p->next;
		}
	}
		break;
	case cslang::NK_Function:
	{
		auto func = std::static_pointer_cast<cslang::ast::FunctionNode>(node);
		traversal_statement(func->stmt, v);
	}
		break;
	}
}

}