#pragma once

#include <memory>
#include <vector>
#include <string>

namespace cslang { struct Node; class Tokenizer; }

namespace codegraph
{

class AstNode
{
public:
	AstNode() {}
	AstNode(const std::shared_ptr<cslang::Tokenizer>& tokenizer,
		const std::shared_ptr<cslang::Node>& node,
		const std::shared_ptr<cslang::Node>& root);

	void Print() const;

	std::string GetName() const;
	void SetName(const std::string& name);

	std::vector<std::shared_ptr<AstNode>> GetChildren() const;
	auto GetRoot() const { return m_root; }
	auto GetNode() const { return m_node; }

	auto GetTokenizer() const { return m_tokenizer; }

private:
	std::shared_ptr<cslang::Tokenizer> m_tokenizer = nullptr;

	std::shared_ptr<cslang::Node> m_node = nullptr;

	std::shared_ptr<cslang::Node> m_root = nullptr;

}; // AstNode

}
