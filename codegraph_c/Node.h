#pragma once

#include <memory>

namespace cslang { struct Node; class Tokenizer; }

namespace codegraph
{

class Node
{
public:
	Node(const std::shared_ptr<cslang::Tokenizer>& tokenizer,
		const std::shared_ptr<cslang::Node>& node);

	void Print() const;

	const char* GetName() const;

private:
	std::shared_ptr<cslang::Tokenizer> m_tokenizer = nullptr;

	std::shared_ptr<cslang::Node> m_node = nullptr;

}; // Node

}
