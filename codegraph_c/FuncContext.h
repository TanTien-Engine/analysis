#pragma once

#include <map>
#include <string>
#include <memory>

namespace codegraph
{

class BasicBlock;

struct FuncContext
{
	std::multimap<std::string, std::shared_ptr<BasicBlock>> gotos;
};

}