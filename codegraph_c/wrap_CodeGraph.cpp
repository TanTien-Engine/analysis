#include "wrap_CodeGraph.h"
#include "Node.h"
#include "modules/script/TransHelper.h"

#include <cslang/Parser.h>

namespace
{

void w_CodeGraph_parse()
{
    const char* str = ves_tostring(1);

    cslang::Parser parser(str);
    auto root = cslang::ast::DeclarationParser::ParseTranslationUnit(parser);

    auto node = std::make_shared<codegraph::Node>(parser.GetTokenizerPtr(), root);

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
    if (strcmp(signature, "static CodeGraph.parse(_)") == 0) return w_CodeGraph_parse;
    if (strcmp(signature, "static CodeGraph.print(_)") == 0) return w_CodeGraph_print;

    return nullptr;
}

void CodeGraphBindClass(const char* class_name, VesselForeignClassMethods* methods)
{
}

}