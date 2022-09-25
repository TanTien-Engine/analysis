#pragma once

#include <vector>
#include <memory>

namespace codegraph
{

class Node;

std::vector<std::shared_ptr<Node>> NodeExpand(const Node& node);

}