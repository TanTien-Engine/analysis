#pragma once

#include <memory>
#include <string>

namespace loggraph
{

class Node;

class Traceback
{
public:
	Traceback(const std::string& filepath,
		const std::shared_ptr<Node>& data);

	uint32_t GetHash() const { return m_hash; }

	auto& GetFilepath() const { return m_filepath; }

	auto& GetData() const { return m_data; }

private:
	std::string m_filepath;

	std::shared_ptr<Node> m_data;

	uint32_t m_hash = 0;

}; // Traceback

}