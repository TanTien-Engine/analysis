#include "wrap_LogGraph.h"
#include "LogParser.h"
#include "Node.h"
#include "modules/script/TransHelper.h"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace
{

void w_Node_allocate()
{
    auto proxy = (tt::Proxy<loggraph::Node>*)ves_set_newforeign(0, 0, sizeof(tt::Proxy<loggraph::Node>));
    proxy->obj = std::make_shared<loggraph::Node>();
}

int w_Node_finalize(void* data)
{
    auto proxy = (tt::Proxy<loggraph::Node>*)(data);
    proxy->~Proxy();
    return sizeof(tt::Proxy<loggraph::Node>);
}

void w_Node_get_name()
{
    auto node = ((tt::Proxy<loggraph::Node>*)ves_toforeign(0))->obj;

    auto& name = node->name;
    ves_set_lstring(0, name.c_str(), name.size());
}

void w_Node_get_children()
{
    auto node = ((tt::Proxy<loggraph::Node>*)ves_toforeign(0))->obj;

    ves_pop(ves_argnum());

    const int num = (int)(node->children.size());
    ves_newlist(num);
    for (int i = 0; i < num; ++i)
    {
        ves_pushnil();
        ves_import_class("loggraph", "Node");
        auto proxy = (tt::Proxy<loggraph::Node>*)ves_set_newforeign(1, 2, sizeof(tt::Proxy<loggraph::Node>));
        proxy->obj = node->children[i];
        ves_pop(1);
        ves_seti(-2, i);
        ves_pop(1);
    }
}

void w_Node_get_items()
{
    auto node = ((tt::Proxy<loggraph::Node>*)ves_toforeign(0))->obj;
    tt::return_list(node->items);
}

void w_Node_has_item()
{
    auto node = ((tt::Proxy<loggraph::Node>*)ves_toforeign(0))->obj;

    auto item = (int)ves_tonumber(1);

    bool find = false;
    for (auto& i : node->items) {
        if (i == item) {
            find = true;
            break;
        }
    }

    ves_set_boolean(0, find);
}

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

void w_LogGraph_parse()
{
    const char* str = ves_tostring(1);

    loggraph::LogParser parser(str);
    parser.Parse();
    auto& nodes = parser.GetNodes();

    ves_pop(ves_argnum());

    const int num = (int)(nodes.size());
    ves_newlist(num);
    for (int i = 0; i < num; ++i)
    {
        ves_pushnil();
        ves_import_class("loggraph", "Node");
        auto proxy = (tt::Proxy<loggraph::Node>*)ves_set_newforeign(1, 2, sizeof(tt::Proxy<loggraph::Node>));
        proxy->obj = nodes[i];
        ves_pop(1);
        ves_seti(-2, i);
        ves_pop(1);
    }
}

}

namespace loggraph
{

VesselForeignMethodFn LogGraphBindMethod(const char* signature)
{
    if (strcmp(signature, "Node.get_name()") == 0) return w_Node_get_name;
    if (strcmp(signature, "Node.get_children()") == 0) return w_Node_get_children;
    if (strcmp(signature, "Node.get_items()") == 0) return w_Node_get_items;
    if (strcmp(signature, "Node.has_item(_)") == 0) return w_Node_has_item;

    if (strcmp(signature, "static LogGraph.split(_)") == 0) return w_LogGraph_split;
    if (strcmp(signature, "static LogGraph.sort(_)") == 0) return w_LogGraph_sort;
    if (strcmp(signature, "static LogGraph.rm_dup(_)") == 0) return w_LogGraph_rm_dup;
    if (strcmp(signature, "static LogGraph.parse(_)") == 0) return w_LogGraph_parse;

    return nullptr;
}

void LogGraphBindClass(const char* class_name, VesselForeignClassMethods* methods)
{
    if (strcmp(class_name, "Node") == 0)
    {
        methods->allocate = w_Node_allocate;
        methods->finalize = w_Node_finalize;
        return;
    }
}

}