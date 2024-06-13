#ifndef CUCKOOHASHING_H
#define CUCKOOHASHING_H

#include <vector>
#include <cstdint>
#include "./i-file-utils.h"
#include "./dni-pos.h"

class CuckooHashing : public IFileUtils
{
private:
    uint32_t lastPos;
    std::vector<DniPos> m_tabla;
    char m_dirArchivo[50];
    uint32_t m_sizeTabla;
    int32_t m_firstHash(DniPos dniPos);
    int32_t m_secondHash(DniPos dniPos);
    void m_rehash(DniPos dniPos, uint32_t pos);
    void m_rehashAll(DniPos dniPos);
    bool writeFile() override;
    bool readFile() override;

public:
    CuckooHashing(uint32_t sizeTabla);
    bool insertDni(DniPos dniPos);
    DniPos searchDNI(uint32_t dni); // Returns the DNI with the position in drive
    uint32_t getSize();
};

#endif // CUCKOOHASHING_H
