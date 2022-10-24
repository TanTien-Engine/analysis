#include "wrap_CodeGraph.h"
#include "Node.h"
#include "BasicBlock.h"
#include "VarAnalysis.h"
#include "AstToCfg.h"
#include "modules/script/TransHelper.h"

#include <cslang/Parser.h>

namespace
{

void w_Node_allocate()
{
    auto proxy = (tt::Proxy<codegraph::Node>*)ves_set_newforeign(0, 0, sizeof(tt::Proxy<codegraph::Node>));
    proxy->obj = std::make_shared<codegraph::Node>();
}

int w_Node_finalize(void* data)
{
    auto proxy = (tt::Proxy<codegraph::Node>*)(data);
    proxy->~Proxy();
    return sizeof(tt::Proxy<codegraph::Node>);
}

void w_Node_get_name()
{
    auto node = ((tt::Proxy<codegraph::Node>*)ves_toforeign(0))->obj;

    auto name = node->GetName();
    ves_set_lstring(0, name.c_str(), name.size());
}

void w_Node_set_name()
{
    auto node = ((tt::Proxy<codegraph::Node>*)ves_toforeign(0))->obj;
    auto name = ves_tostring(1);
    node->SetName(name);
}

void w_Node_get_children()
{
    auto node = ((tt::Proxy<codegraph::Node>*)ves_toforeign(0))->obj;

    auto children = node->GetChildren();
    
    ves_pop(ves_argnum());

    const int num = (int)(children.size());
    ves_newlist(num);
    for (int i = 0; i < num; ++i)
    {
        ves_pushnil();
        ves_import_class("codegraph", "Node");
        auto proxy = (tt::Proxy<codegraph::Node>*)ves_set_newforeign(1, 2, sizeof(tt::Proxy<codegraph::Node>));
        proxy->obj = children[i];
        ves_pop(1);
        ves_seti(-2, i);
        ves_pop(1);
    }
}

void w_Node_get_root()
{
    auto node = ((tt::Proxy<codegraph::Node>*)ves_toforeign(0))->obj;

    auto root = std::make_shared<codegraph::Node>(node->GetTokenizer(), node->GetRoot(), node->GetRoot());

    ves_pop(ves_argnum());

    ves_pushnil();
    ves_import_class("codegraph", "Node");
    auto proxy = (tt::Proxy<codegraph::Node>*)ves_set_newforeign(0, 1, sizeof(tt::Proxy<codegraph::Node>));
    proxy->obj = root;
    ves_pop(1);
}

void w_Node_gen_func_graph()
{
    auto node = ((tt::Proxy<codegraph::Node>*)ves_toforeign(0))->obj;

    codegraph::VarAnalysis analysis(node);

    std::vector<std::shared_ptr<codegraph::Node>> variables, statements;
    analysis.GetNodes(variables, statements);

    ves_pop(ves_argnum());

    ves_newlist(2);
    {
        int num = static_cast<int>(variables.size());
        ves_newlist(num);
        for (int i = 0; i < num; ++i)
        {
            ves_pushnil();
            ves_import_class("codegraph", "Node");
            auto proxy = (tt::Proxy<codegraph::Node>*)ves_set_newforeign(2, 3, sizeof(tt::Proxy<codegraph::Node>));
            proxy->obj = variables[i];
            ves_pop(1);
            ves_seti(-2, i);
            ves_pop(1);
        }
        ves_seti(-2, 0);
        ves_pop(1);
    }
    {
        int num = static_cast<int>(statements.size());
        ves_newlist(num);
        for (int i = 0; i < num; ++i)
        {
            ves_pushnil();
            ves_import_class("codegraph", "Node");
            auto proxy = (tt::Proxy<codegraph::Node>*)ves_set_newforeign(2, 3, sizeof(tt::Proxy<codegraph::Node>));
            proxy->obj = statements[i];
            ves_pop(1);
            ves_seti(-2, i);
            ves_pop(1);
        }
        ves_seti(-2, 1);
        ves_pop(1);
    }
}

void w_Node_gen_flow_graph()
{
    auto node = ((tt::Proxy<codegraph::Node>*)ves_toforeign(0))->obj;

    auto root = codegraph::AstToCfg::Gen(node);

    ves_pop(ves_argnum());
    if (root)
    {
        ves_pushnil();
        ves_import_class("codegraph", "BasicBlock");
        auto proxy = (tt::Proxy<codegraph::BasicBlock>*)ves_set_newforeign(0, 1, sizeof(tt::Proxy<codegraph::BasicBlock>));
        proxy->obj = root;
        ves_pop(1);
    }
    else
    {
        ves_pushnil();
    }
}

void w_BasicBlock_allocate()
{
    auto proxy = (tt::Proxy<codegraph::BasicBlock>*)ves_set_newforeign(0, 0, sizeof(tt::Proxy<codegraph::BasicBlock>));
    proxy->obj = std::make_shared<codegraph::BasicBlock>();
}

int w_BasicBlock_finalize(void* data)
{
    auto proxy = (tt::Proxy<codegraph::BasicBlock>*)(data);
    proxy->~Proxy();
    return sizeof(tt::Proxy<codegraph::BasicBlock>);
}

void w_BasicBlock_get_name()
{
    auto bb = ((tt::Proxy<codegraph::BasicBlock>*)ves_toforeign(0))->obj;

    auto name = bb->GetName();
    ves_set_lstring(0, name.c_str(), name.size());
}

void w_BasicBlock_get_children()
{
    auto bb = ((tt::Proxy<codegraph::BasicBlock>*)ves_toforeign(0))->obj;

    auto& children = bb->GetChildren();

    ves_pop(ves_argnum());

    const int num = (int)(children.size());
    ves_newlist(num);
    for (int i = 0; i < num; ++i)
    {
        ves_pushnil();
        ves_import_class("codegraph", "BasicBlock");
        auto proxy = (tt::Proxy<codegraph::BasicBlock>*)ves_set_newforeign(1, 2, sizeof(tt::Proxy<codegraph::BasicBlock>));
        proxy->obj = children[i];
        ves_pop(1);
        ves_seti(-2, i);
        ves_pop(1);
    }
}

void w_BasicBlock_get_next()
{
    auto bb = ((tt::Proxy<codegraph::BasicBlock>*)ves_toforeign(0))->obj;

    ves_pop(ves_argnum());

    auto next = bb->GetNext();
    if (next)
    {
        ves_pushnil();
        ves_import_class("codegraph", "BasicBlock");
        auto proxy = (tt::Proxy<codegraph::BasicBlock>*)ves_set_newforeign(0, 1, sizeof(tt::Proxy<codegraph::BasicBlock>));
        proxy->obj = next;
        ves_pop(1);
    }
    else
    {
        ves_pushnil();
    }
}

void w_BasicBlock_print()
{
    auto bb = ((tt::Proxy<codegraph::BasicBlock>*)ves_toforeign(0))->obj;
    bb->Print();
}

void w_BasicBlock_is_exit()
{
    auto bb = ((tt::Proxy<codegraph::BasicBlock>*)ves_toforeign(0))->obj;
    auto& nodes = bb->GetNodes();
    if (nodes.empty()) {
        ves_set_boolean(0, true);
    } else {
        auto stmt = std::static_pointer_cast<cslang::ast::StatementNode>(nodes.back());
        ves_set_boolean(0, stmt->kind == cslang::NK_ReturnStatement);
    }
}

void w_BasicBlock_is_loop()
{
    auto bb = ((tt::Proxy<codegraph::BasicBlock>*)ves_toforeign(0))->obj;
    auto& nodes = bb->GetNodes();
    if (nodes.empty()) {
        ves_set_boolean(0, false);
    } else {
        auto stmt = std::static_pointer_cast<cslang::ast::StatementNode>(nodes.back());
        bool is_loop = stmt->kind == cslang::NK_ForStatement || stmt->kind == cslang::NK_WhileStatement;
        ves_set_boolean(0, is_loop);
    }
}

void w_CodeGraph_parse()
{
    const char* str = ves_tostring(1);

    cslang::Parser parser(str);
    auto root = cslang::ast::DeclarationParser::ParseTranslationUnit(parser);

    auto node = std::make_shared<codegraph::Node>(parser.GetTokenizerPtr(), root, root);

    ves_pop(ves_argnum());

    ves_pushnil();
    ves_import_class("codegraph", "Node");
    auto proxy = (tt::Proxy<codegraph::Node>*)ves_set_newforeign(0, 1, sizeof(tt::Proxy<codegraph::Node>));
    proxy->obj = node;
    ves_pop(1);
}

void w_CodeGraph_print()
{
    auto node = ((tt::Proxy<codegraph::Node>*)ves_toforeign(1))->obj;
    node->Print();
}

}

namespace codegraph
{

VesselForeignMethodFn CodeGraphBindMethod(const char* signature)
{
    if (strcmp(signature, "Node.get_name()") == 0) return w_Node_get_name;
    if (strcmp(signature, "Node.set_name(_)") == 0) return w_Node_set_name;
    if (strcmp(signature, "Node.get_children()") == 0) return w_Node_get_children;
    if (strcmp(signature, "Node.get_root()") == 0) return w_Node_get_root;
    if (strcmp(signature, "Node.gen_func_graph()") == 0) return w_Node_gen_func_graph;
    if (strcmp(signature, "Node.gen_flow_graph()") == 0) return w_Node_gen_flow_graph;

    if (strcmp(signature, "BasicBlock.get_name()") == 0) return w_BasicBlock_get_name;
    if (strcmp(signature, "BasicBlock.get_children()") == 0) return w_BasicBlock_get_children;
    if (strcmp(signature, "BasicBlock.get_next()") == 0) return w_BasicBlock_get_next;
    if (strcmp(signature, "BasicBlock.print()") == 0) return w_BasicBlock_print;
    if (strcmp(signature, "BasicBlock.is_exit()") == 0) return w_BasicBlock_is_exit;
    if (strcmp(signature, "BasicBlock.is_loop()") == 0) return w_BasicBlock_is_loop;

    if (strcmp(signature, "static CodeGraph.parse(_)") == 0) return w_CodeGraph_parse;
    if (strcmp(signature, "static CodeGraph.print(_)") == 0) return w_CodeGraph_print;

    return nullptr;
}

void CodeGraphBindClass(const char* class_name, VesselForeignClassMethods* methods)
{
    if (strcmp(class_name, "Node") == 0)
    {
        methods->allocate = w_Node_allocate;
        methods->finalize = w_Node_finalize;
        return;
    }

    if (strcmp(class_name, "BasicBlock") == 0)
    {
        methods->allocate = w_BasicBlock_allocate;
        methods->finalize = w_BasicBlock_finalize;
        return;
    }
}

}