#include "wrap_LogGraph.h"
#include "LogParser.h"
#include "Node.h"
#include "Traceback.h"
#include "Diff.h"
#include "Regex.h"
#include "modules/script/TransHelper.h"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <map>

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

void w_Node_get_type()
{
    auto node = ((tt::Proxy<loggraph::Node>*)ves_toforeign(0))->obj;

    auto& type = node->GetType();
    ves_set_lstring(0, type.c_str(), type.size());
}

void w_Node_get_name()
{
    auto node = ((tt::Proxy<loggraph::Node>*)ves_toforeign(0))->obj;

    auto& name = node->GetName();
    ves_set_lstring(0, name.c_str(), name.size());
}

void w_Node_get_children()
{
    auto node = ((tt::Proxy<loggraph::Node>*)ves_toforeign(0))->obj;

    ves_pop(ves_argnum());

    auto& children = node->GetAllChildren();
    const int num = (int)(children.size());
    ves_newlist(num);
    for (int i = 0; i < num; ++i)
    {
        ves_pushnil();
        ves_import_class("loggraph", "Node");
        auto proxy = (tt::Proxy<loggraph::Node>*)ves_set_newforeign(1, 2, sizeof(tt::Proxy<loggraph::Node>));
        proxy->obj = children[i];
        ves_pop(1);
        ves_seti(-2, i);
        ves_pop(1);
    }
}

void w_Node_get_data()
{
    auto node = ((tt::Proxy<loggraph::Node>*)ves_toforeign(0))->obj;

    ves_pop(ves_argnum());

    auto& data = node->GetAllData();
    const int num = (int)(data.size());
    ves_newlist(num);
    for (int i = 0; i < num; ++i)
    {
        ves_pushnil();
        ves_import_class("loggraph", "Variant");
        auto var = (loggraph::Variant*)ves_set_newforeign(1, 2, sizeof(loggraph::Variant));
        *var = data[i];
        ves_pop(1);
        ves_seti(-2, i);
        ves_pop(1);
    }
}

void w_Node_has_item()
{
    auto node = ((tt::Proxy<loggraph::Node>*)ves_toforeign(0))->obj;

    auto item = (int)ves_tonumber(1);

    bool find = false;
    for (auto& var : node->GetAllData()) {
        if (var.type == loggraph::VarType::Integer && var.i == item) {
            find = true;
            break;
        }
    }

    ves_set_boolean(0, find);
}

void w_Variant_allocate()
{
    loggraph::Variant* var = (loggraph::Variant*)ves_set_newforeign(0, 0, sizeof(loggraph::Variant));
    *var = loggraph::Variant();
}

int w_Variant_finalize(void* data)
{
    return sizeof(loggraph::Variant);
}

void w_Variant_get_type()
{
    auto var = (loggraph::Variant*)ves_toforeign(0);
    std::string type = "unknown";
    switch (var->type)
    {
    case loggraph::VarType::Integer:
    case loggraph::VarType::Double:
        type = "number";
        break;
    case loggraph::VarType::String:
        type = "string";
        break;
    case loggraph::VarType::Group:
        type = "struct";
        break;
    default:
        assert(0);
    }
    ves_set_lstring(0, type.c_str(), type.size());
}

void w_Variant_get_name()
{
    auto var = (loggraph::Variant*)ves_toforeign(0);
    std::string name = "unknown";
    switch (var->type)
    {
    case loggraph::VarType::Integer:
        name = std::to_string(var->i);
        break;
    case loggraph::VarType::Double:
        name = std::to_string(var->d);
        break;
    case loggraph::VarType::String:
        name = (const char*)var->obj;
        break;
    case loggraph::VarType::Group:
        name = ((const loggraph::VarGroup*)var->obj)->name;
        break;
    default:
        assert(0);
    }
    ves_set_lstring(0, name.c_str(), name.size());
}

void w_Variant_to_number()
{
    auto var = (loggraph::Variant*)ves_toforeign(0);
    double num = 0;
    switch (var->type)
    {
    case loggraph::VarType::Integer:
        num = var->i;
        break;
    case loggraph::VarType::Double:
        num = var->d;
        break;
    //default:
    //{
    //    ves_set_nil(0);
    //    return;
    //}
    }
    ves_set_number(0, num);
}

void w_Variant_get_children()
{
    auto var = (loggraph::Variant*)ves_toforeign(0);

    ves_pop(ves_argnum());

    if (var->type != loggraph::VarType::Group) {
        ves_newlist(0);
        return;
    }

    assert(var->obj);
    auto group = reinterpret_cast<const loggraph::VarGroup*>(var->obj);
    auto& children = group->children;
    const int num = (int)(children.size());
    ves_newlist(num);
    for (int i = 0; i < num; ++i)
    {
        ves_pushnil();
        ves_import_class("loggraph", "Variant");
        loggraph::Variant* var = (loggraph::Variant*)ves_set_newforeign(1, 2, sizeof(loggraph::Variant));
        *var = children[i].second;
        ves_pop(1);
        ves_seti(-2, i);
        ves_pop(1);
    }
}

void w_Traceback_print()
{
    auto traceback = ((tt::Proxy<loggraph::Traceback>*)ves_toforeign(0))->obj;

    printf("++ %s\n", traceback->GetFilepath().c_str());

    auto& data = traceback->GetData()->GetAllData();
    for (size_t i = 0, n = data.size(); i < n; )
    {
        assert(data[i].type == loggraph::VarType::String);
        for (int j = 0; j < 3; ++j) {
            printf("%s ", static_cast<const char*>(data[i].obj));
            ++i;
        }
        printf("\n");
    }
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
    const char* data = ves_tostring(1);
    const char* proto = ves_tostring(2);

    loggraph::LogParser parser(data, proto);
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

void w_LogGraph_traceback()
{
    std::vector<std::shared_ptr<loggraph::Node>> nodes;
    tt::list_to_foreigns(1, nodes);

    std::vector<std::shared_ptr<loggraph::Traceback>> ret;
    for (auto p : nodes)
    {
        for (auto& c : p->GetAllChildren())
        {
            if (c->GetType() != "traceback") {
                continue;
            }

            auto tb = std::make_shared<loggraph::Traceback>(p->GetName(), c);
            ret.push_back(tb);
        }
    }

    ves_pop(ves_argnum());

    const int num = (int)(ret.size());
    ves_newlist(num);
    for (int i = 0; i < num; ++i)
    {
        ves_pushnil();
        ves_import_class("loggraph", "Traceback");
        auto proxy = (tt::Proxy<loggraph::Traceback>*)ves_set_newforeign(1, 2, sizeof(tt::Proxy<loggraph::Traceback>));
        proxy->obj = ret[i];
        ves_pop(1);
        ves_seti(-2, i);
        ves_pop(1);
    }
}

void w_LogGraph_unique()
{
    std::vector<std::shared_ptr<loggraph::Traceback>> nodes;
    tt::list_to_foreigns(1, nodes);

    std::vector<std::shared_ptr<loggraph::Traceback>> ret;

    std::map<uint32_t, std::shared_ptr<loggraph::Traceback>> set;
    for (auto tb : nodes)
    {
        auto key = tb->GetHash();
        set.insert({ key, tb });
    }

    for (auto itr : set) {
        ret.push_back(itr.second);
    }

    ves_pop(ves_argnum());

    const int num = (int)(ret.size());
    ves_newlist(num);
    for (int i = 0; i < num; ++i)
    {
        ves_pushnil();
        ves_import_class("loggraph", "Traceback");
        auto proxy = (tt::Proxy<loggraph::Traceback>*)ves_set_newforeign(1, 2, sizeof(tt::Proxy<loggraph::Traceback>));
        proxy->obj = ret[i];
        ves_pop(1);
        ves_seti(-2, i);
        ves_pop(1);
    }
}

void w_LogGraph_select()
{
    std::vector<std::shared_ptr<loggraph::Node>> nodes;
    tt::list_to_foreigns(1, nodes);

    auto needs = tt::list_to_string_array(2);
    auto excepts = tt::list_to_string_array(3);

    std::vector<std::shared_ptr<loggraph::Node>> ret;
    for (auto& node : nodes)
    {
        int n_pass = 0;
        int n_fail = 0;
        for (auto& var : node->GetAllData())
        {
            if (var.type != loggraph::VarType::String) {
                continue;
            }

            std::string item = std::string(static_cast<const char*>(var.obj));

            for (auto& str : needs) {
                if (item == str) {
                    ++n_pass;
                }
            }

            for (auto& str : excepts) {
                if (item == str) {
                    ++n_fail;
                    break;
                }
            }
        }

        if (n_pass == needs.size() && n_fail == 0) {
            ret.push_back(node);
        }
    }

    ves_pop(ves_argnum());

    const int num = (int)(ret.size());
    ves_newlist(num);
    for (int i = 0; i < num; ++i)
    {
        ves_pushnil();
        ves_import_class("loggraph", "Node");
        auto proxy = (tt::Proxy<loggraph::Node>*)ves_set_newforeign(1, 2, sizeof(tt::Proxy<loggraph::Node>));
        proxy->obj = ret[i];
        ves_pop(1);
        ves_seti(-2, i);
        ves_pop(1);
    }
}

void w_LogGraph_diff()
{
    const char* path0 = ves_tostring(1);
    const char* path1 = ves_tostring(2);

    loggraph::Diff::Print(path0, path1);
}

void w_LogGraph_regex_replace()
{
    const char* text = ves_tostring(1);
    const char* re   = ves_tostring(2);
    const char* fmt  = ves_tostring(3);

    auto ret = loggraph::Regex::Replace(text, re, fmt);
    ves_set_lstring(0, ret.c_str(), ret.size());
}

}

namespace loggraph
{

VesselForeignMethodFn LogGraphBindMethod(const char* signature)
{
    if (strcmp(signature, "Node.get_type()") == 0) return w_Node_get_type;
    if (strcmp(signature, "Node.get_name()") == 0) return w_Node_get_name;
    if (strcmp(signature, "Node.get_children()") == 0) return w_Node_get_children;
    if (strcmp(signature, "Node.get_data()") == 0) return w_Node_get_data;
    if (strcmp(signature, "Node.has_item(_)") == 0) return w_Node_has_item;

    if (strcmp(signature, "Variant.get_type()") == 0) return w_Variant_get_type;
    if (strcmp(signature, "Variant.get_name()") == 0) return w_Variant_get_name;
    if (strcmp(signature, "Variant.to_number()") == 0) return w_Variant_to_number;
    if (strcmp(signature, "Variant.get_children()") == 0) return w_Variant_get_children;

    if (strcmp(signature, "Traceback.print()") == 0) return w_Traceback_print;

    if (strcmp(signature, "static LogGraph.split(_)") == 0) return w_LogGraph_split;
    if (strcmp(signature, "static LogGraph.sort(_)") == 0) return w_LogGraph_sort;
    if (strcmp(signature, "static LogGraph.rm_dup(_)") == 0) return w_LogGraph_rm_dup;
    if (strcmp(signature, "static LogGraph.parse(_,_)") == 0) return w_LogGraph_parse;

    if (strcmp(signature, "static LogGraph.traceback(_)") == 0) return w_LogGraph_traceback;
    if (strcmp(signature, "static LogGraph.unique(_)") == 0) return w_LogGraph_unique;
    if (strcmp(signature, "static LogGraph.select(_,_,_)") == 0) return w_LogGraph_select;

    if (strcmp(signature, "static LogGraph.diff(_,_)") == 0) return w_LogGraph_diff;

    if (strcmp(signature, "static LogGraph.regex_replace(_,_,_)") == 0) return w_LogGraph_regex_replace;

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

    if (strcmp(class_name, "Variant") == 0)
    {
        methods->allocate = w_Variant_allocate;
        methods->finalize = w_Variant_finalize;
        return;
    }
}

}