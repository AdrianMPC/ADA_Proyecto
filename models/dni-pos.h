#pragma once
#include <cstdint>

// Struct to save the dni among the position in the hard drive
struct DniPos {
    uint64_t dni; // DESTRUYE TODO EL PROGRAMA POR PROBLEMAS DE PADDING :v
    uint64_t pos; // SERA NECESARIO YA QUE NECESITAMOS MAS DE 2GB DE TAMAÑO EN LA PARTICION

    DniPos(uint32_t d = 0, uint64_t p = 0) : dni(d), pos(p) {}
};

#include <cassert>
static_assert(sizeof(DniPos) == sizeof(uint64_t) + sizeof(uint64_t), "Unexpected padding in DniPos");
