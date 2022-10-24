#include "NodePrint.h"
#include "AstNode.h"

#include <cslang/AST.h>
#include <cslang/Declaration.h>
#include <cslang/GenCode.h>

#include <sstream>
#include <iostream>

namespace codegraph
{

void NodePrint(const AstNode& _node)
{
	auto node = _node.GetNode();
    switch (node->kind)
    {
    case cslang::NK_TranslationUnit:
	{
		auto root = std::static_pointer_cast<cslang::ast::TranslationUnitNode>(node);

		std::stringstream ss;
		cslang::GenTranslationUnit(ss, root);
		std::cout << ss.str() << std::endl;
	}
        break;
    }
}

}