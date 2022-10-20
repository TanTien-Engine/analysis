#pragma once

#include <memory>
#include <string>
#include <vector>

namespace cslang { struct Node; class Tokenizer; }

namespace codegraph
{

class BasicBlock
{
public:
	BasicBlock() {}
	BasicBlock(const std::shared_ptr<cslang::Tokenizer>& tokenizer, const std::string& name);

	auto& GetName() const { return m_name; }

	void Print() const;

	void AddNode(const std::shared_ptr<cslang::Node>& node) { m_nodes.push_back(node); }

	void SetPrev(const std::shared_ptr<BasicBlock>& prev) { m_prev = prev; }
	void SetNext(const std::shared_ptr<BasicBlock>& next) { m_next = next; }

	auto GetNext() const { return m_next; }
	auto GetPrev() const { return m_prev; }

private:
	std::shared_ptr<cslang::Tokenizer> m_tokenizer = nullptr;

	std::vector<std::shared_ptr<cslang::Node>> m_nodes;

	std::string m_name;

	std::shared_ptr<BasicBlock> m_prev = nullptr, m_next = nullptr;

}; // BasicBlock

}