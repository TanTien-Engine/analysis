#define QUOTE(...) #__VA_ARGS__
static const char* loggraphModuleSource = QUOTE(

foreign class Node
{
	init() {}

	foreign get_type()
	foreign get_name()

	foreign get_children()
	foreign get_items()

	foreign has_item(item)

	toString() 
	{
		var type = this.get_type()
		var name = this.get_name()
		return "++ %(type), %(name)"
	}
}

foreign class Traceback
{
	init() {}

	foreign print()
}

foreign class LogGraph
{
	foreign static split(str)
	foreign static sort(items)
	foreign static rm_dup(items)

	foreign static parse(str)

	foreign static traceback(nodes, unique)
	foreign static select(nodes, needs, excepts)
}

);