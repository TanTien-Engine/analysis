#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <set>

namespace loggraph
{

enum class VarType
{
	Invalid = 0,

	Integer,
	Double,
	String,

	Group,
};

struct VarGroup;

struct Variant
{
	Variant();
	Variant(int i);
	Variant(double d);
	Variant(const std::string& s);
	Variant(const VarGroup* group);
	~Variant();

	VarType type;
	union
	{
		int64_t i;
		double  d;
		const void* obj;
	};

private:
	static std::set<std::string> strings;
};

struct VarGroup
{
	std::vector<Variant> children;
	std::vector<std::string> names;
};

}