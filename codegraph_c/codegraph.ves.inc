#define QUOTE(...) #__VA_ARGS__
static const char* codegraphModuleSource = QUOTE(

foreign class Node
{
	init() {}

	foreign get_name()
	foreign set_name(name)

	foreign get_children()
	foreign get_root()
}

foreign class CodeGraph
{
	foreign static parse(str)
	foreign static print(node)
}

);