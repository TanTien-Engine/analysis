#include "FlowGraph.h"
#include "BasicBlock.h"
#include "Node.h"

#include <cslang/AST.h>
#include <cslang/Declaration.h>

namespace
{

void connect_two_list(const std::shared_ptr<codegraph::BasicBlock>& list0, 
	                  const std::shared_ptr<codegraph::BasicBlock>& list1)
{
	auto prev = list0;
	while (prev && prev->GetNext()) {
		prev = prev->GetNext();
	}

	auto next = list1;
	while (next && next->GetPrev()) {
		next = next->GetPrev();
	}

	if (prev) {
		prev->SetNext(next);
	}
	if (next) {
		next->SetPrev(prev);
	}
}

}

namespace codegraph
{

FlowGraph::FlowGraph(const std::shared_ptr<Node>& node)
{
	Build(node);
}

void FlowGraph::Build(const std::shared_ptr<Node>& node)
{
	auto tokenizer = node->GetTokenizer();
	auto root = node->GetNode();

	m_root = std::make_shared<BasicBlock>(tokenizer, "root");

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
				auto node = BuildFunc(tokenizer, func);
				m_root->AddChild(node);
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
}

std::shared_ptr<BasicBlock> 
FlowGraph::BuildFunc(const std::shared_ptr<cslang::Tokenizer>& tokenizer,
	                 const std::shared_ptr<cslang::ast::FunctionNode>& func) const
{
	auto name = func->fdec->dec->id;
	
	auto bb = std::make_shared<BasicBlock>(tokenizer, name);
	bb->AddNode(func);

	auto stmt = BuildStatement(tokenizer, func->stmt);
	connect_two_list(bb, stmt);

	return bb;
}

std::shared_ptr<BasicBlock> 
FlowGraph::BuildStatement(const std::shared_ptr<cslang::Tokenizer>& tokenizer,
	                      const std::shared_ptr<cslang::ast::StatementNode>& stmt) const
{
	auto get_stmt_name = [](const std::shared_ptr<cslang::Node>& node) -> std::string
	{
		auto stmt = std::static_pointer_cast<cslang::ast::StatementNode>(node);

		std::string name;
		switch (stmt->kind)
		{
		case cslang::NK_ExpressionStatement:
			name = "expr";
			break;
		case cslang::NK_LabelStatement:
			name = "label";
			break;
		case cslang::NK_CaseStatement:
			name = "case";
			break;
		case cslang::NK_DefaultStatement:
			name = "default";
			break;
		case cslang::NK_IfStatement:
			name = "if";
			break;
		case cslang::NK_SwitchStatement:
			name = "switch";
			break;
		case cslang::NK_WhileStatement:
			name = "while";
			break;
		case cslang::NK_DoStatement:
			name = "do";
			break;
		case cslang::NK_ForStatement:
			name = "for";
			break;
		case cslang::NK_GotoStatement:
			name = "goto";
			break;
		case cslang::NK_ContinueStatement:
			name = "continue";
			break;
		case cslang::NK_BreakStatement:
			name = "break";
			break;
		case cslang::NK_ReturnStatement:
			name = "return";
			break;
		case cslang::NK_CompoundStatement:
			name = "comp";
			break;
		case cslang::NK_Declaration:
			name = "decl";
			break;
		default:
			assert(0);
		}

		return name;
	};

	if (stmt->kind == cslang::NK_CompoundStatement)
	{
		std::shared_ptr<BasicBlock> head = nullptr, curr = nullptr;
		auto add_bb = [&](const std::shared_ptr<BasicBlock>& bb)
		{
			if (curr)
			{
				connect_two_list(curr, bb);
				curr = bb;
			}
			else
			{
				head = bb;
				curr = bb;
			}
		};

		std::vector<std::shared_ptr<cslang::Node>> merged;
		auto build_merge_bb = [&]() -> std::shared_ptr<BasicBlock>
		{		
			assert(!merged.empty());

			std::shared_ptr<BasicBlock> bb = nullptr;
			if (merged.size() == 1)
			{
				bb = std::make_shared<BasicBlock>(tokenizer, get_stmt_name(merged[0]));
				bb->AddNode(merged[0]);
			}
			else
			{
				auto name = "merged" + std::to_string(merged.size());
				bb = std::make_shared<BasicBlock>(tokenizer, name);
				for (auto& n : merged) {
					bb->AddNode(n);
				}
			}

			merged.clear();

			return bb;
		};

		auto p = std::static_pointer_cast<cslang::ast::CompoundStmtNode>(stmt)->stmts;
		while (p != NULL)
		{
			auto c_stmt = std::static_pointer_cast<cslang::ast::StatementNode>(p);

			if (c_stmt->kind == cslang::NK_ExpressionStatement || c_stmt->kind == cslang::NK_Declaration)
			{
				merged.push_back(c_stmt);
			}
			else
			{
				if (!merged.empty()) {
					add_bb(build_merge_bb());
				}

				add_bb(BuildStatement(tokenizer, c_stmt));
			}

			p = p->next;
		}

		if (!merged.empty()) {
			add_bb(build_merge_bb());
		}

		return head;
	}
	else
	{
		auto bb = std::make_shared<BasicBlock>(tokenizer, get_stmt_name(stmt));
		bb->AddNode(stmt);

		switch (stmt->kind)
		{
		case cslang::NK_IfStatement:
		{
			auto if_stmt = std::static_pointer_cast<cslang::ast::IfStmtNode>(stmt);
			auto then_bb = BuildStatement(tokenizer, if_stmt->then_stmt);
			bb->AddChild(then_bb);
			if (if_stmt->else_stmt) {
				auto else_bb = BuildStatement(tokenizer, if_stmt->else_stmt);
				bb->AddChild(else_bb);
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

			auto body = BuildStatement(tokenizer, for_stmt->stmt);

			//bb->AddChild(header);
			bb->AddChild(body);
		}
			break;
		case cslang::NK_WhileStatement:
		{
			auto loop_stmt = std::static_pointer_cast<cslang::ast::LoopStmtNode>(stmt);

			//auto header = std::make_shared<BasicBlock>(tokenizer, "while");
			//header->AddNode(loop_stmt->expr);

			auto body = BuildStatement(tokenizer, loop_stmt->stmt);

			//bb->AddChild(header);
			bb->AddChild(body);
		}
			break;
		}

		return bb;
	}
}

}