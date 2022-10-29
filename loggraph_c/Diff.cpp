#include "Diff.h"

#include "../external/dtl/dtl/dtl.hpp"

#include <fstream>
#include <vector>

namespace loggraph
{

void Diff::Print(const std::string& path0, const std::string& path1)
{
    typedef std::string       elem;
    typedef std::vector<elem> sequence;
    typedef std::pair<elem, dtl::elemInfo> sesElem;

    std::ifstream      Aifs(path0.c_str());
    std::ifstream      Bifs(path1.c_str());
    elem          buf;
    sequence      ALines, BLines;

    while (std::getline(Aifs, buf)) {
        ALines.push_back(buf);
    }
    while (getline(Bifs, buf)) {
        BLines.push_back(buf);
    }

    dtl::Diff<elem> diff(ALines, BLines);
    diff.onHuge();
    //diff.onUnserious();
    diff.compose();

    // type unihunk definition test
    dtl::uniHunk<sesElem> hunk;

    if (diff.getEditDistance() > 0) {
    //    showStats(fp1, fp2);             // show file info
    }

    diff.composeUnifiedHunks();
    diff.printUnifiedFormat();
}

}