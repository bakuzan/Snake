#pragma once

enum class PendingChange
{
    NONE = 0,
    PUSH,
    POP,
    CHANGE,
    REPLACE,
};