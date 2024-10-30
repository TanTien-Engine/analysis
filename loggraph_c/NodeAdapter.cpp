#include "NodeAdapter.h"
#include "Node.h"

#include <polymesh3/Polytope.h>
#include <graph/Graph.h>
#include <graph/Node.h>
#include <graph/Edge.h>
#include <graph/GraphTools.h>
#include <graph/NodeRank.h>

namespace loggraph
{

std::shared_ptr<pm3::Polytope> 
NodeAdapter::ToPolytope(const std::shared_ptr<Node>& node)
{
	if (node->GetType() != "polytope") {
		return nullptr;
	}
	
	std::vector<pm3::Polytope::PointPtr> verts;
	std::vector<pm3::Polytope::FacePtr>  faces;

	auto& items = node->GetAllData();
	for (auto item : items)
	{
		auto group = reinterpret_cast<const loggraph::VarGroup*>(item.p);
		if (group->name == "vert")
		{
			assert(group->children.size() == 3);
			assert(group->children[0].first == "x" && group->children[0].second.type == VarType::Double
				&& group->children[1].first == "y" && group->children[1].second.type == VarType::Double
				&& group->children[2].first == "z" && group->children[2].second.type == VarType::Double);

			sm::vec3 pos;
			for (int i = 0; i < 3; ++i) {
				pos.xyz[i] = static_cast<float>(group->children[i].second.d);
			}

			verts.push_back(std::make_shared<pm3::Polytope::Point>(pos));
		}
		else if (group->name == "face")
		{
			assert(group->children.size() > 2);

			auto face = std::make_shared<pm3::Polytope::Face>();
			for (auto c : group->children) 
			{
				assert(c.first == "idx" && c.second.type == VarType::Integer);
				face->border.push_back(c.second.i);
			}

			faces.push_back(face);
		}
		else
		{
			assert(0);
		}
	}

	auto poly = std::make_shared<pm3::Polytope>(verts, faces);
	return poly;
}

std::shared_ptr<graph::Graph> 
NodeAdapter::ToGraph(const std::shared_ptr<Node>& log_node)
{
	if (log_node->GetType() == "graph")
	{
		auto graph = std::make_shared<graph::Graph>();

		std::map<int, int> id2idx;

		auto& items = log_node->GetAllData();

		for (auto item : items)
		{
			auto group = reinterpret_cast<const loggraph::VarGroup*>(item.p);
			if (group->name == "node")
			{
				auto node = std::make_shared<graph::Node>();

				for (auto c : group->children)
				{
					if (c.first == "id")
					{
						assert(c.second.type == VarType::Integer);
						node->SetValue(c.second.i);
					}
					else if (c.first == "rank")
					{
						assert(c.second.type == VarType::Integer);
						node->AddComponent<graph::NodeRank>(c.second.i);
					}
				}

				id2idx.insert({ node->GetValue(), graph->GetNodes().size() });

				graph->AddNode(node);
			}
		}

		for (auto item : items)
		{
			auto group = reinterpret_cast<const loggraph::VarGroup*>(item.p);
			if (group->name == "edge")
			{
				assert(group->children.size() == 2);

				auto f = group->children[0];
				auto t = group->children[1];
				assert(f.first == "from" && f.second.type == VarType::Integer);
				assert(t.first == "to" && t.second.type == VarType::Integer);
				auto f_itr = id2idx.find(f.second.i);
				auto t_itr = id2idx.find(t.second.i);
				assert(f_itr != id2idx.end() && t_itr != id2idx.end());
				graph->AddEdge(f_itr->second, t_itr->second);
			}
		}

		return graph;
	}
	else if (log_node->GetType() == "graph2")
	{
		auto graph = std::make_shared<graph::Graph>();

		std::map<int, int> id2idx;

		auto& items = log_node->GetAllData();
		for (auto item : items)
		{
			auto group = reinterpret_cast<const loggraph::VarGroup*>(item.p);
			if (group->name == "node")
			{
				std::vector<int> inputs, outputs;

				auto op_node = std::make_shared<graph::Node>();
				int op_idx = graph->GetNodes().size();
				graph->AddNode(op_node);

				for (auto c : group->children)
				{
					if (c.first == "type")
					{
						assert(c.second.type == VarType::String);
						const char* str = reinterpret_cast<const char*>(c.second.p);
						op_node->SetName(str);
					}
					else if (c.first == "input")
					{
						assert(c.second.type == VarType::Integer);
						inputs.push_back(c.second.i);
					}
					else if (c.first == "output")
					{
						assert(c.second.type == VarType::Integer);
						outputs.push_back(c.second.i);
					}
				}

				for (auto id : inputs)
				{
					auto itr = id2idx.find(id);
					assert(itr != id2idx.end());
					graph->AddEdge(itr->second, op_idx);
				}
				for (auto id : outputs)
				{
					auto entity_node = std::make_shared<graph::Node>();
					int entity_idx = graph->GetNodes().size();
					graph->AddNode(entity_node);
					graph->AddEdge(op_idx, entity_idx);

					entity_node->SetValue(id);
					id2idx.insert({ id, entity_idx });
				}
			}
		}

		return graph;
	}

	return nullptr;
}

}
