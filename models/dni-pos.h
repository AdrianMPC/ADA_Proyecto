#pragma once
#include <cstdint>

// Struct to save the dni among the position in the hard drive
struct DniPos {
    uint32_t dni;
    int64_t pos; // SERA NECESARIO YA QUE NECESITAMOS MAS DE 2GB DE TAMAÑO EN LA PARTICION

    DniPos(uint32_t d = 0, uint32_t p = -1) : dni(d), pos(p) {}
};
