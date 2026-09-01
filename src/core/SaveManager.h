#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "data/SaveData.h"
#include "data/SaveMeta.h"

class SaveManager
{
private:
    std::string baseDir;

private:
    std::string slotFilename(int slot) const;

    // Helpers
    std::string encodeRLE(const std::vector<std::vector<bool>> &grid);
    std::vector<std::vector<bool>> decodeRLE(
        const std::string &str, int width, int height);

    template <typename T>
    static std::string join(const std::vector<T> &values, char sep = ',');

    template <typename T>
    static std::vector<T> split(const std::string &s, char sep = ',');

    // Enum <-> int adapters
    template <typename Enum>
    static std::vector<int> enumToInts(const std::vector<Enum> &values);

    template <typename Enum>
    static std::vector<Enum> intsToEnum(const std::vector<int> &ints);

public:
    SaveManager(const std::string &dir);
    ~SaveManager();

    SaveMeta loadMeta(int slot);

    void saveSlot(int slot, const SaveData &data);
    SaveData loadSlot(int slot);
};

#endif // SAVEMANAGER_H
