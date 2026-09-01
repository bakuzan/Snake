#pragma once

#include <string>

struct SaveMeta
{
    bool occupied;
    std::string room;
    std::string locationName;
    std::string timestamp;

    static SaveMeta makeDefault()
    {
        SaveMeta m;
        m.occupied = false;
        m.room = "";
        m.locationName = "";
        m.timestamp = "";
        return m;
    }
};