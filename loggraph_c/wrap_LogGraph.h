#pragma once

#include <vessel.h>

namespace loggraph
{

VesselForeignMethodFn LogGraphBindMethod(const char* signature);
void LogGraphBindClass(const char* class_name, VesselForeignClassMethods* methods);

}