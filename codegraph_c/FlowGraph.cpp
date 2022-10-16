#include "FlowGraph.h"
#include "BasicBlock.h"
#include "Node.h"

#include <cslang/AST.h>
#include <cslang/Declaration.h>

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
			//case NK_Declaration:
			//	GenVariant(output, std::static_pointer_cast<DeclarationNode>(p), 0);
			//	LeftAlign(output, 0);
			//	break;
			case cslang::NK_Function:
			{
				auto name = std::static_pointer_cast<cslang::ast::FunctionNode>(p)->fdec->dec->id;
				auto bb = std::make_shared<BasicBlock>(tokenizer, p, name);
				m_nodes.push_back(bb);
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

	int zz = 0;
}

}