#pragma once

#include <memory>
#include <vector>

namespace codegraph
{

class BasicBlock;

class BBlockTools
{
public:
	static bool IsLoop(const std::shared_ptr<BasicBlock>& bb);
	static bool IsGoto(const std::shared_ptr<BasicBlock>& bb);
	static bool IsLabel(const std::shared_ptr<BasicBlock>& bb);

private:
	static bool CheckType(const std::shared_ptr<BasicBlock>& bb, const std::vector<int>& types);

}; // BBlockTools

}