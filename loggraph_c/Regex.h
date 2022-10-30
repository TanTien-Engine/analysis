#pragma once

#include <string>

namespace loggraph
{

class Regex
{
public:
	static std::string Replace(const std::string& text, 
		const std::string& re, const std::string& fmt);

}; // Regex

}