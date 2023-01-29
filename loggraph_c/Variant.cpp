#include "Variant.h"

namespace loggraph
{

std::set<std::string> Variant::strings;

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
	auto itr = strings.find(s);
	if (itr == strings.end()) {
		auto itr = strings.insert(s);
		obj = itr.first->data();
	}
	else {
		obj = itr->data();
	}
}

Variant::Variant(const VarGroup* group)
	: type(VarType::Group)
	, obj(group)
{
}

Variant::~Variant()
{
	//if (type == VarType::Group)
	//{
	//	auto children = reinterpret_cast<const VarGroup*>(obj);
	//	delete children;
	//	obj = nullptr;
	//}
}

}