#include "BBlockBuilder.h"
#include "BasicBlock.h"
#include "AstToCfg.h"

#include <cslang/Statement.h>

namespace
{

class ListBB
{
public:
	void Add(const std::shared_ptr<codegraph::BasicBlock>& bb)
	{
		if (m_curr)
		{
			codegraph::BBlockBuilder::Connect(m_curr, bb);
			m_curr = bb;
		}
		else
		{
			m_head = bb;
			m_curr = bb;
		}
	};

	auto GetHead() const { return m_head; }

private:
	std::shared_ptr<codegraph::BasicBlock> m_head = nullptr, m_curr = nullptr;

}; // ListBB

}

namespace codegraph
{

void BBlockBuilder::Connect(const std::shared_ptr<codegraph::BasicBlock>& list0,
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

std::shared_ptr<codegraph::BasicBlock>
BBlockBuilder::Merge(const std::shared_ptr<cslang::Tokenizer>& tokenizer, std::vector<std::shared_ptr<cslang::Node>>& merged)
{
	assert(!merged.empty());

	std::shared_ptr<codegraph::BasicBlock> bb = nullptr;
	if (merged.size() == 1)
	{
		auto stat = std::static_pointer_cast<cslang::ast::StatementNode>(merged[0]);
		bb = std::make_shared<codegraph::BasicBlock>(tokenizer, GetName(stat));
		bb->AddNode(merged[0]);
	}
	else
	{
		auto name = "merged" + std::to_string(merged.size());
		bb = std::make_shared<codegraph::BasicBlock>(tokenizer, name);
		for (auto& n : merged) {
			bb->AddNode(n);
		}
	}

	merged.clear();

	return bb;
}

std::shared_ptr<codegraph::BasicBlock>
BBlockBuilder::Split(const std::shared_ptr<cslang::Tokenizer>& tokenizer, const std::shared_ptr<cslang::ast::CompoundStmtNode>& comp)
{
	ListBB list_bb;

	std::vector<std::shared_ptr<cslang::Node>> merged;

	auto p = comp->stmts;
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
				list_bb.Add(Merge(tokenizer, merged));
			}

			list_bb.Add(AstToCfg::GenStat(tokenizer, c_stmt));
		}

		p = p->next;
	}

	if (!merged.empty()) {
		list_bb.Add(Merge(tokenizer, merged));
	}

	return list_bb.GetHead();
}

std::vector<std::shared_ptr<codegraph::BasicBlock>>
BBlockBuilder::Split(const std::shared_ptr<cslang::Tokenizer>& tokenizer, const std::shared_ptr<cslang::ast::SwitchStmtNode>& switch_stmt)
{
	std::vector<std::shared_ptr<codegraph::BasicBlock>> ret;

	std::vector<std::shared_ptr<cslang::Node>> merged;

	auto p = std::static_pointer_cast<cslang::ast::CompoundStmtNode>(switch_stmt->stmt)->stmts;
	while (p != NULL)
	{
		auto c_stmt = std::static_pointer_cast<cslang::ast::StatementNode>(p);

		merged.push_back(c_stmt);
		if (c_stmt->kind == cslang::NK_BreakStatement) {			
			ret.push_back(Merge(tokenizer, merged));
		}

		p = p->next;
	}

	if (!merged.empty()) {
		ret.push_back(Merge(tokenizer, merged));
	}

	return ret;
}

std::string BBlockBuilder::GetName(const std::shared_ptr<cslang::ast::StatementNode>& node)
{
	std::string name;
	switch (node->kind)
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
}

}