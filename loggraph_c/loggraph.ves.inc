#define QUOTE(...) #__VA_ARGS__
static const char* loggraphModuleSource = QUOTE(

foreign class Node
{
	init() {}

	foreign get_type()
	foreign get_name()

	foreign get_children()
	foreign get_data()

	foreign has_item(item)

	toString() 
	{
		var type = this.get_type()
		var name = this.get_name()
		return "++ %(type), %(name)"
	}
}

foreign class Variant
{
	init() {}

	foreign get_type()
	foreign get_name()

	foreign to_number()

	foreign get_children()
}

foreign class NodeAdapter
{
	foreign static to_polytope(node)
	foreign static to_graph(node)
}

foreign class Traceback
{
	init() {}

	foreign print()
}

foreign class Graph2
{
	init() {}

	foreign get_nodes()
	foreign get_edges()

	foreign query_node(x, y)

	foreign layout()
}

foreign class GraphNode
{
	init() {}

	foreign get_pos()
	foreign set_pos(x, y)
}

foreign class LogGraph
{
	foreign static split(str)
	foreign static sort(items)
	foreign static rm_dup(items)

	foreign static parse(data, proto)

	foreign static traceback(nodes)
	foreign static unique(nodes)
	foreign static select(nodes, needs, excepts)

	foreign static diff(path0, path1)

	foreign static regex_replace(text, re, fmt)
}

);