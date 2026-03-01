#include <iostream>
#include <vector>
#include <set>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <span>
#include "file.hpp"
#include "string.hpp"

namespace LightBFSPathLib
{
    using namespace std;
    using namespace shizuku::io;
    using namespace shizuku::util::string;
    namespace fs = std::filesystem;
    using Graph = map<string, set<string>>;
    using Path = fs::path;

    class LightBFSPath
    {
    private:
        Graph _graph;
        Path _tablePath;
        ifstream _tablereader;
        bool _isInstance = false;

    public:
        LightBFSPath(Path tablePath) : _tablePath(move(tablePath))
        {
            if (!File::Exists(_tablePath))
            {
                throw std::runtime_error("File not exist: " + _tablePath.string());
                return;
            }
            _isInstance = true;
        }
        bool ReadGraph()
        {
            vector<string> v = File::ReadAllLines(_tablePath);
            for (const auto &s : v)
            {
                auto lineFull = Split(s, ' ');
                if (lineFull.size() < 3)
                {
                    throw std::runtime_error("Each line shall contain at least 2 elements excluding the connection mode identifier: " + s);
                    return false;
                }
                auto lineElement = span(lineFull).subspan(1);
                if (lineFull[0] == "+")
                {
                    // 行内元素全连接
                    for (size_t i = 0; i < lineElement.size(); ++i)
                    {
                        for (size_t j = i + 1; j < lineElement.size(); ++j)
                        {
                            const auto &a = lineElement[i];
                            const auto &b = lineElement[j];
                            _graph[a].insert(b);
                            _graph[b].insert(a);
                        }
                    }
                }
                else if (lineFull[0] == "-")
                {
                    // 行内除连接方式标识符以外的所有元素与第一个元素单向连接 方向:第一个元素->别的元素
                    std::string beginPos = lineElement[0];
                    for (const auto &endPos : span(lineElement).subspan(1))
                    {
                        _graph[beginPos].insert(endPos);
                    }
                }
                else
                {
                    throw std::runtime_error("Syntax error in connecting graph elements: " + s);
                    return false;
                }
            }
            return true;
        }
    };

}
