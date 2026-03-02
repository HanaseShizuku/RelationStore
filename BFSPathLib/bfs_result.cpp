#include <map>
#include <string>
#include "bfs_result.h"
namespace BFSPathLib
{
    BFSResult::BFSResult(std::map<std::string, NodeInfo> resultMap):_resultMap(std::move(resultMap)){}
}
