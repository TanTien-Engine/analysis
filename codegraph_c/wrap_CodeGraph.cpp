#include "wrap_CodeGraph.h"

#include <cslang/Parser.h>
#include <cslang/GenCode.h>

#include <cstring>
#include <iostream>

namespace
{

void w_CodeGraph_parse()
{
    const char* str = ves_tostring(1);

    cslang::Parser parser(str);
    auto stat = cslang::ast::DeclarationParser::ParseTranslationUnit(parser);

    std::stringstream ss;
    cslang::GenTranslationUnit(ss, stat);
    std::cout << ss.str() << std::endl;
}

}

namespace codegraph
{

VesselForeignMethodFn CodeGraphBindMethod(const char* signature)
{
    if (strcmp(signature, "static CodeGraph.parse(_)") == 0) return w_CodeGraph_parse;

    return nullptr;
}

void CodeGraphBindClass(const char* class_name, VesselForeignClassMethods* methods)
{
}

}