#pragma once

#include <memory>
#include <string>

namespace cslang { struct Node; class Tokenizer; }

namespace codegraph
{

class BasicBlock
{
public:
	BasicBlock() {}
	BasicBlock(const std::shared_ptr<cslang::Tokenizer>& tokenizer,
		const std::shared_ptr<cslang::Node>& node, const std::string& name);

	auto& GetName() const { return m_name; }

	void Print() const;

private:
	std::shared_ptr<cslang::Tokenizer> m_tokenizer = nullptr;

	std::shared_ptr<cslang::Node> m_node = nullptr;

	std::string m_name;

}; // BasicBlock

}