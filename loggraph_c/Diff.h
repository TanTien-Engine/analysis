#pragma once

#include <string>

namespace loggraph
{

class Diff
{
public:
	static void Print(const std::string& path0, const std::string& path1);

}; // Diff

}