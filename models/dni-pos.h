#pragma once
#include <cstdint>

// Struct to save the dni among the position in the hard drive
struct DniPos {
    uint32_t dni;
    int32_t pos;

    DniPos(uint32_t d = 0, int32_t p = -1) : dni(d), pos(p) {}
};