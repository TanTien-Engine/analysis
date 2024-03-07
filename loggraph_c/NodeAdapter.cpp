#include "NodeAdapter.h"
#include "Node.h"

#include <polymesh3/Polytope.h>
#include <graph/Graph.h>
#include <graph/Node.h>
#include <graph/Edge.h>
#include <graph/GraphTools.h>

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
		auto group = reinterpret_cast<const loggraph::VarGroup*>(item.obj);
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
NodeAdapter::ToGraph(const std::shared_ptr<Node>& node)
{
	if (node->GetType() != "graph") {
		return nullptr;
	}

	auto graph = std::make_shared<graph::Graph>();

	std::map<int, int> id2idx;

	auto& items = node->GetAllData();

	for (auto item : items)
	{
		auto group = reinterpret_cast<const loggraph::VarGroup*>(item.obj);
		if (group->name == "node")
		{
			assert(group->children.size() == 1);

			auto node = std::make_shared<graph::Node>();

			auto c = group->children[0];
			assert(c.first == "id" && c.second.type == VarType::Integer);
			node->SetId(c.second.i);

			id2idx.insert({ node->GetId(), graph->GetNodes().size() });

			graph->AddNode(node);
		}
	}

	for (auto item : items)
	{
		auto group = reinterpret_cast<const loggraph::VarGroup*>(item.obj);
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

}
