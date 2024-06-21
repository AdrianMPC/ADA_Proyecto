#ifndef CUCKOOHASHING_H
#define CUCKOOHASHING_H

#include <vector>
#include <cstdint>
#include <fstream>
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
    bool readHashItem(std::ifstream& file, DniPos& item);   
    static int32_t hashFunction(uint32_t key, uint32_t tableSize) {
        const uint32_t prime = 0x9e3779b1; // A large prime number (Knuth's constant for mixing)
        key ^= (key >> 21);
        key *= prime;
        key ^= (key >> 24);
        key += (key << 3);
        key ^= (key >> 11);
        key += (key << 15);
        key ^= (key >> 17);
        return key % tableSize;
	}

public:
	const uint32_t DEFAULT_DNI_VALUE = 0; // el compilador se queja de que short no es bueno
	const uint32_t DEFAULT_POSITION_VALUE = 0;
    CuckooHashing(uint32_t sizeTabla);
    bool insertDni(DniPos dniPos);
    DniPos searchDNI(uint32_t dni); // Returns the DNI with the position in drive
    bool deleteDNI(uint32_t dni);
    uint32_t getSize();
    bool doesTableExists();
    bool writeFile() override;
    bool readFile() override;
    uint32_t getlastPos();
    void setlastPos(uint32_t _newlastpos);
    void printVector(uint32_t max);
};

#endif // CUCKOOHASHING_H
