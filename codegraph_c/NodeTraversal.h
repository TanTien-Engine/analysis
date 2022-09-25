#pragma once

#include <memory>

namespace cslang { struct Node; }
namespace cslang::ast { 
	struct DeclarationNode; 
	struct ExpressionNode; 
	struct StatementNode;
	struct FunctionNode;
}

namespace codegraph
{

class Node;

class Visitor
{
public:
	virtual void VisitVariant(const std::shared_ptr<cslang::ast::DeclarationNode>& var) {}
	virtual void VisitExpression(const std::shared_ptr<cslang::ast::ExpressionNode>& expr) {}
	virtual void VisitStatement(const std::shared_ptr<cslang::ast::StatementNode>& stat) {}
	virtual void VisitFunction(const std::shared_ptr<cslang::ast::FunctionNode>& func) {}
	virtual ~Visitor() = default;
}; // Visitor

void Traversal(const std::shared_ptr<cslang::Node>& node, Visitor& v);

}