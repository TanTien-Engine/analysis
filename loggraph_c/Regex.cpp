#include "Regex.h"

#include <regex>

namespace loggraph
{

std::string Regex::Replace(const std::string& text, const std::string& re, const std::string& fmt)
{
	return std::regex_replace(text, std::regex(re), fmt);
}

}