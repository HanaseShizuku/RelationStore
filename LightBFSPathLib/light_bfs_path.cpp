#include <iostream>
#include <vector>
#include <set>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>

#define PUBLIC public:
#define PRIVATE private:

namespace LightBFSPathLib
{
    using namespace std;
    namespace fs = std::filesystem;
    using Graph = map<string, set<string>>;
    using Path = fs::path;

    class LightBFSPath
    {
        PRIVATE Graph _graph;
        PRIVATE Path _tablePath;
        PRIVATE ifstream _tablereader;
        PUBLIC LightBFSPath(Path tablePath) : _tablePath(move(tablePath))
        {
            if (!fs::exists(_tablePath))
            {
                throw fs::filesystem_error(
                    "Cannot open patch table",
                    _tablePath,
                    std::make_error_code(std::errc::no_such_file_or_directory));
                _tablereader=ifstream(_tablePath);
            }
        }
        PUBLIC bool ReadGraph()
        {
        }
    };

    void PatchGraph(Graph &graph, fs::path patchTablePath)
    {
        if (!fs::exists(patchTablePath))
        {
            return;
        }
        std::string line;
        ifstream patchTable(patchTablePath);
        if (!patchTable.is_open())
        {
            return;
        }
        while (getline(patchTable, line))
        {
            stringstream ss(line);
            string word;
            vector<string> table;
            while (ss >> word)
            {
                table.push_back(word);
            }
            if (table.size() < 2)
            {
                continue;
            }
            for (auto &a : table)
            {
                for (auto &b : table)
                {
                    if (a != b)
                    {
                        graph[a].insert(b);
                        graph[b].insert(a);
                    }
                }
            }
        }
    }

}

#undef PRIVATE
#undef PUBLIC