#pragma once

#include <vector>
#include <cstdint>
#include "./i-file-utils.h"
#include "./dni-pos.h"

class CuckooHashing : public IFileUtils {
    private:
        std::vector<DniPos> m_tabla;
        char m_dirArchivo[50]; 
        uint32_t m_sizeTabla;
        int32_t m_firstHash(DniPos dniPos);
        int32_t m_secondHash(DniPos dniPos);
        void m_rehash(DniPos dniPos, uint32_t pos);
        void m_rehashAll(DniPos dniPos);
    public:
        CuckooHashing(uint32_t sizeTabla);
        bool insertDni(DniPos dniPos);
        int32_t searchDNI(uint32_t dni);
        uint32_t getSize();
        bool writeFile() override;
        bool readFile() override;
};

uint32_t CuckooHashing::getSize() {
    return this->m_sizeTabla;
}
