#include <filesystem>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <iostream>

#include "SaveManager.h"

SaveManager::SaveManager(const std::string &dir)
    : baseDir(dir)
{
    // Constructor
}

SaveManager::~SaveManager()
{
    // Destructor
}

// Publics

SaveMeta SaveManager::loadMeta(int slot)
{
    std::ifstream file(slotFilename(slot));

    if (!file.is_open())
    {
        return SaveMeta::makeDefault();
    }

    SaveMeta meta;
    std::unordered_map<std::string, std::string> kv;
    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string key, value;

        if (std::getline(iss, key, '=') &&
            std::getline(iss, value))
        {
            kv[key] = value;
        }
    }

    // Load items

    return meta;
}

void SaveManager::saveSlot(int slot, const SaveData &data)
{
    std::string filename = slotFilename(slot);
    std::ofstream file(filename, std::ios::trunc);

    if (!file.is_open())
    {
        std::cerr << "[SaveManager] Could not open save file for writing: "
                  << filename << "\n";
        return;
    }

    // Save items
    (void)data;

    file.close();
}

SaveData SaveManager::loadSlot(int slot)
{
    std::string filename = slotFilename(slot);
    std::ifstream file(filename);

    if (!file.is_open())
    {
        return SaveData::makeDefault();
    }

    SaveData data;
    std::unordered_map<std::string, std::string> kv;
    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string key, value;

        if (std::getline(iss, key, '=') &&
            std::getline(iss, value))
        {
            kv[key] = value;
        }
    }

    // Load items

    return data;
}

// Privates

std::string SaveManager::slotFilename(int slot) const
{
    std::filesystem::create_directories(baseDir);
    return baseDir + "/save" + std::to_string(slot) + ".sav";
}

// Helpers

template <typename T>
std::string SaveManager::join(const std::vector<T> &values, char sep)
{
    std::ostringstream oss;

    for (size_t i = 0; i < values.size(); ++i)
    {
        oss << values[i];
        if (i + 1 < values.size())
        {
            oss << sep;
        }
    }

    return oss.str();
}

template <typename T>
std::vector<T> SaveManager::split(const std::string &s, char sep)
{
    std::vector<T> result;
    std::istringstream iss(s);
    std::string token;

    while (std::getline(iss, token, sep))
    {
        if (!token.empty())
        {
            std::istringstream valStream(token);
            T value;
            valStream >> value;
            result.push_back(value);
        }
    }

    return result;
}

template <typename Enum>
std::vector<int> SaveManager::enumToInts(const std::vector<Enum> &values)
{
    static_assert(std::is_enum_v<Enum>, "enumToInts requires an enum type");

    std::vector<int> out;
    out.reserve(values.size());

    for (auto v : values)
    {
        out.push_back(static_cast<int>(v));
    }

    return out;
}

template <typename Enum>
std::vector<Enum> SaveManager::intsToEnum(const std::vector<int> &ints)
{
    static_assert(std::is_enum_v<Enum>, "intsToEnum requires an enum type");

    std::vector<Enum> out;
    out.reserve(ints.size());

    for (int v : ints)
    {
        out.push_back(static_cast<Enum>(v));
    }

    return out;
}

std::string SaveManager::encodeRLE(const std::vector<std::vector<bool>> &grid)
{
    std::ostringstream oss;

    for (const auto &row : grid)
    {
        int count = 1;
        bool prev = row[0];

        for (int i = 1; i < row.size(); ++i)
        {
            if (row[i] == prev)
                count++;
            else
            {
                oss << count << "x" << (prev ? "1" : "0") << ",";
                prev = row[i];
                count = 1;
            }
        }

        oss << count << "x" << (prev ? "1" : "0") << ";"; // end of row
    }

    return oss.str();
}

std::vector<std::vector<bool>> SaveManager::decodeRLE(
    const std::string &str, int width, int height)
{
    std::vector<std::vector<bool>> grid(height, std::vector<bool>(width, false));

    int x = 0;
    int y = 0;

    auto rows = split<std::string>(str, ';');

    for (auto &rowStr : rows)
    {
        if (rowStr.empty())
        {
            continue;
        }

        auto tokens = split<std::string>(rowStr, ',');

        for (auto &tok : tokens)
        {
            auto pos = tok.find('x');
            int count = std::stoi(tok.substr(0, pos));
            bool val = tok[pos + 1] == '1';

            for (int i = 0; i < count; ++i)
            {
                if (x >= width)
                {
                    x = 0;
                    y++;
                    if (y >= height)
                    {
                        return grid;
                    }
                }

                grid[y][x] = val;
                x++;
            }
        }

        x = 0;
        y++;
        if (y >= height)
        {
            break;
        }
    }

    return grid;
}
