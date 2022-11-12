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
	auto& GetNodes() const { return m_nodes; }

	void AddChild(const std::shared_ptr<BasicBlock>& child) {
		m_children.push_back(child);
	}
	auto& GetChildren() const { return m_children; }

	void SetPrev(const std::shared_ptr<BasicBlock>& prev) { m_prev = prev; }
	void SetNext(const std::shared_ptr<BasicBlock>& next) { m_next = next; }

	auto GetNext() const { return m_next; }
	auto GetPrev() const { return m_prev; }

	void AddInput(const std::shared_ptr<BasicBlock>& bb) { m_input.push_back(bb); }
	void AddOutput(const std::shared_ptr<BasicBlock>& bb) { m_output.push_back(bb); }

	auto& GetOutput() const { return m_output; }

	auto GetTokenizer() const { return m_tokenizer; }

	auto GetTarget() const { return m_target; }
	auto SetTarget(const std::shared_ptr<BasicBlock>& target) { m_target = target; }

private:
	std::shared_ptr<cslang::Tokenizer> m_tokenizer = nullptr;

	std::vector<std::shared_ptr<cslang::Node>> m_nodes;

	std::string m_name;

	std::shared_ptr<BasicBlock> m_prev = nullptr, m_next = nullptr;
	std::vector<std::shared_ptr<BasicBlock>> m_children;

	std::vector<std::shared_ptr<BasicBlock>> m_input, m_output;
	std::shared_ptr<BasicBlock> m_target = nullptr;

}; // BasicBlock

}