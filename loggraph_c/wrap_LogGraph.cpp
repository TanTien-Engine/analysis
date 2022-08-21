#include "wrap_LogGraph.h"
#include "modules/script/TransHelper.h"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace
{

std::vector<const char*> list_to_strings(int index)
{
    std::vector<const char*> ret;
    if (ves_type(index) != VES_TYPE_LIST) {
        return ret;
    }

    const int num = ves_len(index);
    ret.reserve(num);
    for (int i = 0; i < num; ++i)
    {
        ves_geti(index, i);
        ret.push_back(ves_tostring(-1));
        ves_pop(1);        
    }

    return ret;
}

void w_LogGraph_split()
{
    const char* src_str = ves_tostring(1);

    char delimiters;
    if (strchr(src_str, '\n')) {
        delimiters = '\n';
    } else {
        delimiters = ' ';
    }

    std::istringstream ss(src_str);
    std::vector<std::string> items;
    std::string item;
    while (getline(ss, item, delimiters)) {
        items.push_back(item);
    }

	ves_pop(ves_argnum());

	const int num = (int)(items.size());
	ves_newlist(num);
	for (int i = 0; i < num; ++i)
	{
        ves_pushstring(items[i].c_str());
        ves_seti(-2, i);
        ves_pop(1);
	}
}

void w_LogGraph_sort()
{
    auto items = list_to_strings(1);

    std::sort(std::begin(items), std::end(items),
        [](const char* a, const char* b) { return strcmp(a, b) < 0; }
    );
    
	ves_pop(ves_argnum());

	const int num = (int)(items.size());
	ves_newlist(num);
	for (int i = 0; i < num; ++i)
	{
        ves_pushstring(items[i]);
        ves_seti(-2, i);
        ves_pop(1);
	}
}

void w_LogGraph_rm_dup()
{
    auto items = list_to_strings(1);

    std::sort(std::begin(items), std::end(items),
        [](const char* a, const char* b) { return strcmp(a, b) < 0; }
    );
    items.erase(std::unique(items.begin(), items.end(),
        [](const char* a, const char* b) { return strcmp(a, b) == 0; }
    ), items.end());

	ves_pop(ves_argnum());

	const int num = (int)(items.size());
	ves_newlist(num);
	for (int i = 0; i < num; ++i)
	{
        ves_pushstring(items[i]);
        ves_seti(-2, i);
        ves_pop(1);
	}
}

}

namespace loggraph
{

VesselForeignMethodFn LogGraphBindMethod(const char* signature)
{
    if (strcmp(signature, "static LogGraph.split(_)") == 0) return w_LogGraph_split;
    if (strcmp(signature, "static LogGraph.sort(_)") == 0) return w_LogGraph_sort;
    if (strcmp(signature, "static LogGraph.rm_dup(_)") == 0) return w_LogGraph_rm_dup;

    return nullptr;
}

void LogGraphBindClass(const char* class_name, VesselForeignClassMethods* methods)
{
}

}