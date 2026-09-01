#pragma once

struct SaveData
{
    std::string timestamp;

    static SaveData makeDefault()
    {
        SaveData d;
        d.timestamp = "";
        return d;
    }

    static SaveData makeSandbox()
    {
        SaveData d = SaveData::makeDefault();
        return d;
    }

    // Helper
};