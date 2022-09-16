#define QUOTE(...) #__VA_ARGS__
static const char* codegraphModuleSource = QUOTE(

foreign class CodeGraph
{
	foreign static parse(str)
}

);