#pragma once
#include <sstream>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

namespace shizuku
{
    namespace fs = std::filesystem;
}

namespace shizuku::io
{

    class File
    {
    private:
        using Path = std::filesystem::path;

    protected:
    public:
        static bool Exists(const Path &filePath)
        {
            return fs::exists(filePath);
        }
        static std::string ReadAllText(const Path &filePath)
        {
            if (!Exists(filePath))
            {
                throw std::runtime_error("File not exist: " + filePath.string());
            }
            std::ifstream reader(filePath);
            if (!reader.is_open())
            {
                reader.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            }
            std::stringstream ss;
            ss << reader.rdbuf();
            return ss.str();
        }
        static std::vector<std::string> ReadAllLines(const Path &filePath)
        {
            if (!Exists(filePath))
            {
                throw std::runtime_error("File not exist: " + filePath.string());
            }
            std::ifstream reader(filePath);
            if (!reader.is_open())
            {
                reader.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            }
            std::stringstream ss;
            std::vector<std::string> lines;
            std::string line;
            while (std::getline(reader, line))
            {
                lines.push_back(line);
            }
            return lines;
        }
        static bool WriteAllBytes(const Path &filePath, const std::vector<uint8_t> &bytes)
        {
            std::ofstream writer(filePath, std::ios::out | std::ios::binary | std::ios::trunc);
            if (!writer.is_open())
            {
                throw std::runtime_error("Failed to open file for writing: " + filePath.string());
            }
            writer.exceptions(std::ofstream::failbit | std::ofstream::badbit);
            writer.write(reinterpret_cast<const char *>(bytes.data()), bytes.size());
            writer.close();
            return writer.good();
        }

        static bool WriteAllLines(const Path &filePath, const std::vector<std::string> &lines)
        {
            std::ofstream writer(filePath, std::ios::out | std::ios::trunc);
            if (!writer.is_open())
            {
                throw std::runtime_error("Failed to open file for writing: " + filePath.string());
            }

            for (size_t i = 0; i < lines.size(); ++i)
            {
                writer << lines[i];
                if (i != lines.size() - 1)
                {
                    writer << '\n';
                }
            }

            return writer.good();
        }

        static bool WriteAllText(const Path &filePath, const std::string &str)
        {
            std::ofstream writer(filePath, std::ios::out | std::ios::trunc);
            if (!writer.is_open())
            {
                throw std::runtime_error("Failed to open file for writing: " + filePath.string());
            }

            writer << str;

            writer.flush();
            return writer.good();
        }
    };
} // namespace shizuku::io
