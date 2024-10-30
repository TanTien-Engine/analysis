#include "Variant.h"

namespace
{

const char* copy_string(const char* str)
{
	size_t len = strlen(str) + 1;
	char* copy = new char[len];
	strcpy_s(copy, len, str);
	return copy;
}

void clear_var(loggraph::Variant& var)
{
	if (var.type == loggraph::VarType::Group)
	{
		auto children = reinterpret_cast<const loggraph::VarGroup*>(var.p);
		delete children;
	}
	else if (var.type == loggraph::VarType::String)
	{
		auto str = reinterpret_cast<const char*>(var.p);
		delete[] str;
	}
	var.type = loggraph::VarType::Invalid;
}

}

namespace loggraph
{

Variant::Variant()
	: type(VarType::Invalid)
{
}

Variant::Variant(int i)
	: type(VarType::Integer)
	, i(i)
{
}

Variant::Variant(double d)
	: type(VarType::Double)
	, d(d)
{
}

Variant::Variant(const std::string& s)
	: type(VarType::String)
{
	p = copy_string(s.c_str());
}

Variant::Variant(const VarGroup& group)
	: type(VarType::Group)
	, p(new VarGroup(group))
{
}

Variant::Variant(const Variant& var)
{
	this->operator = (var);
}

Variant& Variant::operator = (const Variant& var)
{
	if (&var == this)
		return *this;

	clear_var(*this);

	type = var.type;
	if (var.type == VarType::Group) 
	{
		p = new VarGroup(*static_cast<const VarGroup*>(var.p));
	}
	else if (var.type == VarType::String)
	{
		p = copy_string(static_cast<const char*>(var.p));
	}
	else 
	{
		memcpy(this, &var, sizeof(Variant));
	}
	return *this;
}

Variant::~Variant()
{
	clear_var(*this);
}

}