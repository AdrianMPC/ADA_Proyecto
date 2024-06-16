#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "../models/cuckohashing.h"
#include "../models/dni-pos.h"

CuckooHashing::CuckooHashing(uint32_t sizeTabla) : m_sizeTabla(sizeTabla) {
    m_tabla.resize(sizeTabla, {0, -1});
}

int32_t CuckooHashing::m_firstHash(DniPos dniPos) {
    return dniPos.dni % m_sizeTabla;
}

int32_t CuckooHashing::m_secondHash(DniPos dniPos) {
    return (dniPos.dni / m_sizeTabla) % m_sizeTabla;
}

uint32_t CuckooHashing::getSize() {
    return this->m_sizeTabla;
}

void CuckooHashing::m_rehash(DniPos dniPos, uint32_t pos) {
    int loopCount = 0;
    while (loopCount < m_sizeTabla) {
        std::swap(dniPos, m_tabla[pos]);
        pos = m_firstHash(dniPos);
        if (m_tabla[pos].pos == -1) {
            m_tabla[pos] = dniPos;
            return;
        }
        std::swap(dniPos, m_tabla[pos]);
        pos = m_secondHash(dniPos);
        if (m_tabla[pos].pos == -1) {
            m_tabla[pos] = dniPos;
            return;
        }
        loopCount++;
    }

    // If we end up here, we need to rehash the entire table
    std::cout << "[CuckooHashing] Rehashing required!" << std::endl;
    m_rehashAll(dniPos);
    insertDni(dniPos); // Reinsert the key after rehashing
}

void CuckooHashing::m_rehashAll(DniPos dniPos) {
    std::vector<DniPos> oldTable = m_tabla;

    m_sizeTabla *= 2;
    m_tabla.clear();
    m_tabla.resize(m_sizeTabla, {0, -1});

    for (DniPos key : oldTable) {
        if (key.pos != -1) {
            insertDni(key);
        }
    }
}

bool CuckooHashing::insertDni(const DniPos dniPos) {
    DniPos check = searchDNI(dniPos.dni);
    if(check.dni != 0){std::cerr<<"[CuckooHashing] ya existe el DNI\n"; return false;} // Ya existe
   // std::cout<<"[CUCKOOHASH-INSERTDNI] TEST DNIPOS\n"<< dniPos.dni << " - " << dniPos.pos << "\n";
    int pos1 = m_firstHash(dniPos);
    if (m_tabla[pos1].pos == -1) {
        m_tabla[pos1] = dniPos;
       // std::cout<<"[m_tabla] TEST DNIPOS\n"<< m_tabla[pos1].dni << " - " << m_tabla[pos1].pos << "\n";
        return true;
    }

    DniPos oldKey = m_tabla[pos1];
    m_tabla[pos1] = dniPos;

    int pos2 = m_secondHash(oldKey);
    if (m_tabla[pos2].pos == -1) {
        m_tabla[pos2] = oldKey;
        //std::cout<<"[m_tabla2] TEST DNIPOS\n"<< m_tabla[pos2].dni << " - " << m_tabla[pos2].pos << "\n";
        return true;
    }

    m_rehash(oldKey, pos2);
    return true;
}

DniPos CuckooHashing::searchDNI(uint32_t dni) {
    int pos1 = m_firstHash({dni, 0});
    if (m_tabla[pos1].dni == dni) {
        return m_tabla[pos1];
    }

    int pos2 = m_secondHash({dni, 0});
    if (m_tabla[pos2].dni == dni) {
        return m_tabla[pos2];
    }

    return {0,0}; // Not found
}

bool CuckooHashing::writeFile() {
    std::ofstream file("cuckohash.bin", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[CuckooHashing] Error opening file\n";
        return false;
    }

    // Write the size of the hash table
    uint32_t size = getSize();
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));

    // Write each element of the hash table
    for (const auto& item : m_tabla) {
        file.write(reinterpret_cast<const char*>(&item), sizeof(item));
    }

    file.close();
    return true;
}

bool CuckooHashing::readFile() {
    std::ifstream file("cuckohash.bin", std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "[CuckooHashing] Error opening file\n";
            return false;
        }

        // Leer el tamaño de la tabla hash
        uint32_t size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size));
        m_tabla.resize(size);

        // Leer cada elemento de la tabla hash
        for (DniPos& item : m_tabla) {
            if (!readHashItem(file, item)) {
                std::cerr << "[CuckooHashing] Error reading hash item\n";
                file.close();
                return false;
            }
        }

        file.close();
        return true;
}

bool CuckooHashing::readHashItem(std::ifstream& file, DniPos& item) {
        // Leer cada campo individualmente
        if (!file.read(reinterpret_cast<char*>(&item.dni), sizeof(item.dni))) {
            return false;
        }
        if (!file.read(reinterpret_cast<char*>(&item.pos), sizeof(item.pos))) {
            return false;
        }
        return true;
   }

bool CuckooHashing::doesTableExists(){
    std::ifstream file("cuckohash.bin", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[CuckooHashing] Error opening file\n";
        return false;
    }
    return true;
}

uint32_t CuckooHashing::getlastPos(){
    return lastPos;
}

void CuckooHashing::setlastPos(uint32_t _newlastpos){
    lastPos = _newlastpos;
} 

void CuckooHashing::printVector(uint32_t max){
	int i = 0;
	for(DniPos item : m_tabla){
		if(i > max){return;}
		std::cout<<"DNI: "<< item.dni << " POS:" << item.pos << std::endl;
		i++;
	}
}
