#include "BBlockConnect.h"
#include "BasicBlock.h"
#include "BBlockTools.h"

#include <queue>

namespace
{

void connect(const std::shared_ptr<codegraph::BasicBlock>& bb0, const std::shared_ptr<codegraph::BasicBlock>& bb1)
{
    // skip connect goto, only use target
    if (codegraph::BBlockTools::IsGoto(bb0)) {
        return;
    }

    bb0->AddOutput(bb1);
    bb1->AddInput(bb0);
}

}

namespace codegraph
{

void BBlockConnect::Connect(const std::shared_ptr<BasicBlock>& root)
{
    auto& children = root->GetChildren();
    for (auto child : children) {
        BuildGraph(child);
    }
}

std::pair<std::shared_ptr<BasicBlock>, std::shared_ptr<BasicBlock>>
BBlockConnect::BuildGraph(const std::shared_ptr<BasicBlock>& entry)
{
    std::shared_ptr<BasicBlock> first_node = nullptr, last_node = nullptr;
    std::shared_ptr<BasicBlock> prev_node = nullptr;

	auto curr = entry;
	while (curr)
	{
        auto next_node = curr;

        if (!first_node) {
            first_node = next_node;
        }
        last_node = next_node;

        if (prev_node)
        {
            auto prev_children = prev_node->GetChildren();
            if (prev_children.empty())
            {
                connect(prev_node, next_node);
            }
            else
            {
                auto end_nodes = BuildSubgraph(prev_node, prev_children);
                ConnectNTo1(end_nodes, next_node, prev_node);
            }
        }

        prev_node = next_node;

        curr = curr->GetNext();
	}

    if (prev_node)
    {
        auto prev_children = prev_node->GetChildren();
        if (!prev_children.empty()) {
            BuildSubgraph(prev_node, prev_children);
        }
    }

    return std::make_pair(first_node, last_node);
}

std::vector<std::shared_ptr<BasicBlock>> 
BBlockConnect::BuildSubgraph(const std::shared_ptr<BasicBlock>& prev,
                             const std::vector<std::shared_ptr<BasicBlock>>& children)
{
    std::vector<std::shared_ptr<BasicBlock>> end_nodes;

    auto is_loop = BBlockTools::IsLoop(prev);
    for (auto& child : children)
    {
        auto pair = BuildGraph(child);

        auto next = pair.first;
        connect(prev, next);

        end_nodes.push_back(pair.second);

        //if (is_loop) {
        //    connect(pair.second, prev);
        //}
    }

    return end_nodes;
}

void BBlockConnect::ConnectNTo1(const std::vector<std::shared_ptr<BasicBlock>>& prev_nodes,
                                const std::shared_ptr<BasicBlock>& next_node, 
                                const std::shared_ptr<BasicBlock>& prev_node)
{
    if (prev_nodes.empty() || !next_node) {
        return;
    }

    std::queue<std::shared_ptr<BasicBlock>> buf;
    for (auto node : prev_nodes) {
        buf.push(node);
    }

    while (!buf.empty())
    {
        auto node = buf.front(); buf.pop();

        bool is_end = true;

        auto& output = node->GetOutput();
        for (auto& c : output) 
        {
            buf.push(c);
            is_end = false;
        }

        if (is_end) {
            connect(node, next_node);
        }
    }
}

}