#include "NodeAdapter.h"
#include "Node.h"

#include <polymesh3/Polytope.h>

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

}
