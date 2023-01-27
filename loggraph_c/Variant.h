#pragma once

#include <cstdint>

namespace loggraph
{

enum class VarType
{
	Invalid = 0,

	Integer,
	Double,
	String,

	Struct,
};

struct Variant
{
	VarType type;
	union
	{
		int64_t i;
		double  d;
		const void* obj;
	};
};

}