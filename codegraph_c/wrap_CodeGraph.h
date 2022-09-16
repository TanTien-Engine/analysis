#pragma once

#include <vessel.h>

namespace codegraph
{

VesselForeignMethodFn CodeGraphBindMethod(const char* signature);
void CodeGraphBindClass(const char* class_name, VesselForeignClassMethods* methods);

}