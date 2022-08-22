#define QUOTE(...) #__VA_ARGS__
static const char* loggraphModuleSource = QUOTE(

foreign class Node
{
	init() {}

	foreign get_name()
	foreign get_children()
	foreign get_items()

	foreign has_item(item)

	toString() 
	{
		var name  = this.get_name()
		var items = this.get_items()
		return "%(name)(%(items.count))"
	}
}

foreign class LogGraph
{
	foreign static split(str)
	foreign static sort(items)
	foreign static rm_dup(items)

	foreign static parse(str)
}

);