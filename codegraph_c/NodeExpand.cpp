#include "NodeExpand.h"
#include "Node.h"

#include <cslang/AST.h>
#include <cslang/Declaration.h>

namespace
{

void expand_trans_unit(const std::shared_ptr<cslang::Tokenizer>& tokenizer,
                       const std::shared_ptr<cslang::ast::TranslationUnitNode>& parent,
                       std::vector<std::shared_ptr<codegraph::Node>>& children,
	                   const std::shared_ptr<cslang::Node>& root)
{
    auto p = parent->extDecls;
    while (p)
    {
        std::shared_ptr<cslang::Node> child = nullptr;

        switch (p->kind)
        {
        case cslang::NK_Declaration:
            child = std::static_pointer_cast<cslang::ast::DeclarationNode>(p);
            break;
        case cslang::NK_Function:
            child = std::static_pointer_cast<cslang::ast::FunctionNode>(p);
            break;
        }

        if (child) {
            children.push_back(std::make_shared<codegraph::Node>(tokenizer, child, root));
        }

        p = p->next;
    }
}

void expand_expression(const std::shared_ptr<cslang::Tokenizer>& tokenizer,
                       const std::shared_ptr<cslang::ast::ExpressionNode>& expr,
                       std::vector<std::shared_ptr<codegraph::Node>>& children,
	                   const std::shared_ptr<cslang::Node>& root)
{
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
        expand_expression(tokenizer, expr->kids[0], children, root);
        expand_expression(tokenizer, expr->kids[1], children, root);
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
        expand_expression(tokenizer, expr->kids[0], children, root);
        expand_expression(tokenizer, expr->kids[1], children, root);
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
		expand_expression(tokenizer, expr->kids[0], children, root);
		expand_expression(tokenizer, expr->kids[1], children, root);
		break;
    case cslang::ast::OP_INDEX:
        expand_expression(tokenizer, expr->kids[0], children, root);
        expand_expression(tokenizer, expr->kids[1], children, root);
        break;

	case cslang::ast::OP_CALL:
    {
		children.push_back(std::make_shared<codegraph::Node>(tokenizer, expr, root));

		//cslang::ast::ExprNodePtr p = expr->kids[1];

  //      expand_expression(tokenizer, expr->kids[0], children);
  //      while (p)
  //      {
  //          expand_expression(tokenizer, p, children);
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
		expand_expression(tokenizer, expr->kids[0], children, root);
		break;

	case cslang::ast::OP_CAST:
		expand_expression(tokenizer, expr->kids[0], children, root);
		break;

	case cslang::ast::OP_SIZEOF:
		if (expr->kids[0]->kind == cslang::NK_Expression) {
			expand_expression(tokenizer, expr->kids[0], children, root);
		}
		break;

	case cslang::ast::OP_MEMBER:
	case cslang::ast::OP_PTR_MEMBER:
		expand_expression(tokenizer, expr->kids[0], children, root);
		break;

	case cslang::ast::OP_QUESTION:
		expand_expression(tokenizer, expr->kids[0], children, root);
		expand_expression(tokenizer, expr->kids[1]->kids[0], children, root);
		expand_expression(tokenizer, expr->kids[1]->kids[1], children, root);
		break;

	case cslang::ast::OP_POSTINC:
	case cslang::ast::OP_POSTDEC:
		expand_expression(tokenizer, expr->kids[0], children, root);
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

void expand_variant(const std::shared_ptr<cslang::Tokenizer>& tokenizer,
                    const std::shared_ptr<cslang::ast::DeclarationNode>& decl,
                    std::vector<std::shared_ptr<codegraph::Node>>& children)
{
}

void expand_statement(const std::shared_ptr<cslang::Tokenizer>& tokenizer,
                      const std::shared_ptr<cslang::ast::StatementNode>& stmt,
                      std::vector<std::shared_ptr<codegraph::Node>>& children,
	                  const std::shared_ptr<cslang::Node>& root)
{
	switch (stmt->kind)
	{
	case cslang::NK_ExpressionStatement:
		expand_expression(tokenizer, std::static_pointer_cast<cslang::ast::ExprStmtNode>(stmt)->expr, children, root);
		break;

	case cslang::NK_LabelStatement:
		expand_statement(tokenizer, std::static_pointer_cast<cslang::ast::LabelStmtNode>(stmt)->stmt, children, root);
		break;

	case cslang::NK_CaseStatement:
		expand_expression(tokenizer, std::static_pointer_cast<cslang::ast::CaseStmtNode>(stmt)->expr, children, root);
		expand_statement(tokenizer, std::static_pointer_cast<cslang::ast::CaseStmtNode>(stmt)->stmt, children, root);
		break;

	case cslang::NK_DefaultStatement:
		expand_statement(tokenizer, std::static_pointer_cast<cslang::ast::DefaultStmtNode>(stmt)->stmt, children, root);
		break;

	case cslang::NK_IfStatement:
		expand_expression(tokenizer, std::static_pointer_cast<cslang::ast::IfStmtNode>(stmt)->expr, children, root);
		expand_statement(tokenizer, std::static_pointer_cast<cslang::ast::IfStmtNode>(stmt)->then_stmt, children, root);
		if (std::static_pointer_cast<cslang::ast::IfStmtNode>(stmt)->else_stmt != NULL) {
			expand_statement(tokenizer, std::static_pointer_cast<cslang::ast::IfStmtNode>(stmt)->else_stmt, children, root);
		}
		break;

	case cslang::NK_SwitchStatement:
		expand_expression(tokenizer, std::static_pointer_cast<cslang::ast::SwitchStmtNode>(stmt)->expr, children, root);
		expand_statement(tokenizer, std::static_pointer_cast<cslang::ast::SwitchStmtNode>(stmt)->stmt, children, root);
		break;

	case cslang::NK_WhileStatement:
		expand_expression(tokenizer, std::static_pointer_cast<cslang::ast::LoopStmtNode>(stmt)->expr, children, root);
		expand_statement(tokenizer, std::static_pointer_cast<cslang::ast::LoopStmtNode>(stmt)->stmt, children, root);
		break;

	case cslang::NK_DoStatement:
        expand_statement(tokenizer, std::static_pointer_cast<cslang::ast::LoopStmtNode>(stmt)->stmt, children, root);
		expand_expression(tokenizer, std::static_pointer_cast<cslang::ast::LoopStmtNode>(stmt)->expr, children, root);
		break;

	case cslang::NK_ForStatement:
        expand_variant(tokenizer, std::static_pointer_cast<cslang::ast::ForStmtNode>(stmt)->init_decl, children);
		expand_expression(tokenizer, std::static_pointer_cast<cslang::ast::ForStmtNode>(stmt)->expr, children, root);
		expand_expression(tokenizer, std::static_pointer_cast<cslang::ast::ForStmtNode>(stmt)->incr_expr, children, root);
		expand_statement(tokenizer, std::static_pointer_cast<cslang::ast::ForStmtNode>(stmt)->stmt, children, root);
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
		expand_expression(tokenizer, std::static_pointer_cast<cslang::ast::ReturnStmtNode>(stmt)->expr, children, root);
		break;

	case cslang::NK_CompoundStatement:
		{
			auto p = std::static_pointer_cast<cslang::ast::CompoundStmtNode>(stmt)->stmts;
			while (p != NULL) {
				expand_statement(tokenizer, std::static_pointer_cast<cslang::ast::StatementNode>(p), children, root);
				p = p->next;
			}
			break;
		}

    case cslang::NK_Declaration:
        expand_variant(tokenizer, std::reinterpret_pointer_cast<cslang::ast::DeclarationNode>(stmt), children);
        break;

	default:
		assert(0);
	}
}

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
    std::vector<std::shared_ptr<Node>> children;

    auto node = _node.GetNode();
    if (!node) {
        return children;
    }

	auto tokenizer = _node.GetTokenizer();
    auto root = _node.GetRoot();

    switch (node->kind)
    {
    case cslang::NK_TranslationUnit:
    {
        auto root = std::static_pointer_cast<cslang::ast::TranslationUnitNode>(node);
        expand_trans_unit(tokenizer, root, children, root);
    }
        break;
    case cslang::NK_Function:
    {
		auto func = std::static_pointer_cast<cslang::ast::FunctionNode>(node);
		expand_statement(tokenizer, func->stmt, children, root);
    }
        break;
    }

    return children;
}

}