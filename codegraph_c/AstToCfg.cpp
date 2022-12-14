#include "AstToCfg.h"
#include "BBlockBuilder.h"
#include "BBlockConnect.h"
#include "BasicBlock.h"
#include "AstNode.h"
#include "FuncContext.h"

#include <cslang/Statement.h>
#include <cslang/Declaration.h>

namespace codegraph
{

std::shared_ptr<BasicBlock> AstToCfg::Gen(const std::shared_ptr<AstNode>& node)
{
	auto tokenizer = node->GetTokenizer();
	auto root = node->GetNode();

	auto ret = std::make_shared<BasicBlock>(tokenizer, "root");

	switch (root->kind)
	{
	case cslang::NK_TranslationUnit:
	{
		auto tu = std::static_pointer_cast<cslang::ast::TranslationUnitNode>(root);
		auto p = tu->extDecls;
		while (p)
		{
			switch (p->kind)
			{
			case cslang::NK_Function:
			{
				auto func = std::static_pointer_cast<cslang::ast::FunctionNode>(p);
				auto node = GenFunc(tokenizer, func);
				ret->AddChild(node);
			}
				break;
			default:
				assert(0);
			}
			p = p->next;
		}
	}
		break;
	default:
		assert(0);
	}

	BBlockConnect::Connect(ret);

	return ret;
}

std::shared_ptr<BasicBlock>
AstToCfg::GenFunc(const std::shared_ptr<cslang::Tokenizer>& tokenizer, const std::shared_ptr<cslang::ast::FunctionNode>& func)
{
	auto name = func->fdec->dec->id;

	auto bb = std::make_shared<BasicBlock>(tokenizer, name);
	bb->AddNode(func);

	FuncContext ctx;

	auto stmt = GenStat(tokenizer, func->stmt, ctx);
	BBlockBuilder::Connect(bb, stmt);

	return bb;
}

std::shared_ptr<BasicBlock>
AstToCfg::GenStat(const std::shared_ptr<cslang::Tokenizer>& tokenizer, const std::shared_ptr<cslang::ast::StatementNode>& stmt, FuncContext& ctx)
{
	if (stmt->kind == cslang::NK_CompoundStatement)
	{
		auto comp_stat = std::static_pointer_cast<cslang::ast::CompoundStmtNode>(stmt);
		return BBlockBuilder::Split(tokenizer, comp_stat, ctx);
	}

	auto bb = std::make_shared<BasicBlock>(tokenizer, BBlockBuilder::GetName(stmt));
	bb->AddNode(stmt);

	switch (stmt->kind)
	{
	case cslang::NK_IfStatement:
	{
		auto if_stmt = std::static_pointer_cast<cslang::ast::IfStmtNode>(stmt);
		auto then_bb = GenStat(tokenizer, if_stmt->then_stmt, ctx);
		bb->AddChild(then_bb);
		if (if_stmt->else_stmt) {
			auto else_bb = GenStat(tokenizer, if_stmt->else_stmt, ctx);
			bb->AddChild(else_bb);
		} else {
			auto dummy = std::make_shared<BasicBlock>(tokenizer, "dummy");
			bb->AddChild(dummy);
		}
	}
		break;
	case cslang::NK_SwitchStatement:
	{
		auto switch_stmt = std::static_pointer_cast<cslang::ast::SwitchStmtNode>(stmt);

		auto children = BBlockBuilder::Split(tokenizer, switch_stmt);
		for (auto& child : children) {
			bb->AddChild(child);
		}
	}
		break;
	case cslang::NK_ForStatement:
	{
		auto for_stmt = std::static_pointer_cast<cslang::ast::ForStmtNode>(stmt);

		//auto header = std::make_shared<BasicBlock>(tokenizer, "for");
		//header->AddNode(for_stmt->init_decl);
		//header->AddNode(for_stmt->expr);
		//header->AddNode(for_stmt->incr_expr);

		auto body = GenStat(tokenizer, for_stmt->stmt, ctx);

		//bb->AddChild(header);
		bb->AddChild(body);
	}
		break;
	case cslang::NK_WhileStatement:
	case cslang::NK_DoStatement:
	{
		auto loop_stmt = std::static_pointer_cast<cslang::ast::LoopStmtNode>(stmt);

		//auto header = std::make_shared<BasicBlock>(tokenizer, "while");
		//header->AddNode(loop_stmt->expr);

		auto body = GenStat(tokenizer, loop_stmt->stmt, ctx);

		//bb->AddChild(header);
		bb->AddChild(body);
	}
		break;
	}

	return bb;
}

}