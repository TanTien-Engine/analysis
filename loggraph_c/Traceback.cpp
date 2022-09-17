#include "Traceback.h"
#include "Node.h"

#include <string>

namespace
{

template <class T>
inline void hash_combine(uint32_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

}

namespace loggraph
{

Traceback::Traceback(const std::string& filepath,
	                 const std::shared_ptr<Node>& data)
	: m_filepath(filepath)
	, m_data(data)
	, m_hash(0)
{
	for (auto& str : data->strings) {
		hash_combine(m_hash, str);
	}
}

}