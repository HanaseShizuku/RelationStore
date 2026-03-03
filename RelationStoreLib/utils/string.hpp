#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

namespace shizuku::util::string
{
    // powered by gemini

    // 1. Trim: 移除首尾空白字符
    inline std::string Trim(const std::string &str)
    {
        if (str.empty())
            return str;
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos)
            return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }

    // 2. Replace: 替换所有指定的子串
    inline std::string Replace(std::string str, const std::string &oldValue, const std::string &newValue)
    {
        if (oldValue.empty())
            return str;
        size_t pos = 0;
        while ((pos = str.find(oldValue, pos)) != std::string::npos)
        {
            str.replace(pos, oldValue.length(), newValue);
            pos += newValue.length();
        }
        return str;
    }

    // 3. Split: 根据分隔符拆分字符串
    inline std::vector<std::string> Split(const std::string &str, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        while (std::getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }
        return tokens;
    }

    // 4. ToUpper: 转大写
    inline std::string ToUpper(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        return str;
    }

    // 5. ToLower: 转小写
    inline std::string ToLower(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }

    // 6. Contains: 是否包含子串 (C# 常用)
    inline bool Contains(const std::string &str, const std::string &value)
    {
        return str.find(value) != std::string::npos;
    }

    // 7. StartsWith: 是否以某字符串开头
    inline bool StartsWith(const std::string &str, const std::string &prefix)
    {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }

    // 8. EndsWith: 是否以某字符串结尾
    inline bool EndsWith(const std::string &str, const std::string &suffix)
    {
        return str.size() >= suffix.size() &&
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    // 9. Join: 像 C# String.Join 一样拼接 (额外赠送，非常实用)
    inline std::string Join(const std::vector<std::string> &elements, const std::string &separator)
    {
        std::string result;
        for (size_t i = 0; i < elements.size(); ++i)
        {
            result += elements[i];
            if (i < elements.size() - 1)
                result += separator;
        }
        return result;
    }
    namespace view
    {
        /**
         * @brief 零拷贝裁剪。
         * @warning 返回的 string_view 指向原字符串内存。若原字符串被销毁，此视图失效。
         */
        inline std::string_view Trim(std::string_view str)
        {
            size_t first = str.find_first_not_of(" \t\n\r");
            if (first == std::string_view::npos)
                return "";
            size_t last = str.find_last_not_of(" \t\n\r");
            return str.substr(first, (last - first + 1));
        }

        /**
         * @brief 零拷贝切分。
         * @return 返回包含多个视图的 vector，不产生子字符串拷贝。
         */
        inline std::vector<std::string_view> Split(std::string_view str, char delimiter)
        {
            std::vector<std::string_view> tokens;
            size_t start = 0, end = 0;
            while ((end = str.find(delimiter, start)) != std::string_view::npos)
            {
                tokens.push_back(str.substr(start, end - start));
                start = end + 1;
            }
            tokens.push_back(str.substr(start));
            return tokens;
        }

        /**
         * @brief 原地转大写 (In-place)。
         * @warning 直接修改传入的字符串，不返回新对象。
         */
        inline void ToUpperInPlace(std::string &str)
        {
            std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        }

        inline std::string FromView(std::string_view view)
        {
            return std::string{view};
        }
    }

} // namespace shizuku::util::string