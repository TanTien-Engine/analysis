#define QUOTE(...) #__VA_ARGS__
static const char* codegraphModuleSource = QUOTE(

foreign class Node
{
	init() {}
}

foreign class CodeGraph
{
	foreign static parse(str)
	foreign static print(node)
}

);