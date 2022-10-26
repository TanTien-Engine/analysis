#define QUOTE(...) #__VA_ARGS__
static const char* codegraphModuleSource = QUOTE(

foreign class Node
{
	init() {}

	foreign get_name()
	foreign set_name(name)

	foreign get_children()
	foreign get_root()

	foreign gen_func_graph()
	foreign gen_flow_graph()
}

foreign class BasicBlock
{
	init() {}

	foreign get_name()
	foreign get_children()

	foreign get_next()

	foreign print()

	foreign is_loop()
	foreign is_dummy()
	foreign is_goto()
	foreign is_label()

	foreign get_label()
}

foreign class CodeGraph
{
	foreign static parse(str)
	foreign static print(node)
}

);