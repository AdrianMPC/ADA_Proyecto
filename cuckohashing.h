#pragma once

#include <vector>
#include <cstdint>
#include "IFileUtils.h"

class CuckoHashing : IFileUtils {
    private:
        std::vector<int32_t> m_tabla;
        char m_dirArchivo[50]; 
        uint32_t m_sizeTabla;
        int32_t m_firstHash(uint32_t dni);
        int32_t m_secondHash(uint32_t dni);
        void m_rehash(uint32_t dni, uint32_t pos);
        void m_rehashAll(uint32_t dni);
    public:
        CuckoHashing(uint32_t sizeTabla);
        bool insertDni(uint32_t dni);
        uint32_t searchDNI(uint32_t dni);
        bool writeFile() override;
        bool readFile() override;
};