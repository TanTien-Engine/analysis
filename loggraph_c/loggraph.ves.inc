#define QUOTE(...) #__VA_ARGS__
static const char* loggraphModuleSource = QUOTE(

foreign class LogGraph
{
	foreign static split(str)
	foreign static sort(items)
	foreign static rm_dup(items)
}

);