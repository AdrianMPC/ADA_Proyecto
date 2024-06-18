#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "../models/cuckohashing.h"
#include "../models/personamodelo.h"
#include "../models/dni-pos.h"


CuckooHashing::CuckooHashing(uint32_t sizeTabla) : m_sizeTabla(sizeTabla) {
    m_tabla.resize(sizeTabla, {DEFAULT_DNI_VALUE, DEFAULT_POSITION_VALUE});
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
        if (m_tabla[pos].dni == DEFAULT_POSITION_VALUE) {
            m_tabla[pos] = dniPos;
            return;
        }
        std::swap(dniPos, m_tabla[pos]);
        pos = m_secondHash(dniPos);
        if (m_tabla[pos].dni == DEFAULT_POSITION_VALUE) {
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
    m_tabla.resize(m_sizeTabla, {DEFAULT_DNI_VALUE, DEFAULT_POSITION_VALUE});

    for (DniPos key : oldTable) {
        if (key.pos != DEFAULT_POSITION_VALUE) {
            insertDni(key);
        }
    }
}

bool CuckooHashing::insertDni(const DniPos dniPos) {
    DniPos check = searchDNI(dniPos.dni);
    if(check.dni != DEFAULT_DNI_VALUE){std::cerr<<"[CuckooHashing] ya existe el DNI\n"; return false;}
    int pos1 = m_firstHash(dniPos);
    if (m_tabla[pos1].pos == DEFAULT_POSITION_VALUE) { 
        m_tabla[pos1] = dniPos;
        return true;
    }

    DniPos oldKey = m_tabla[pos1];
    m_tabla[pos1] = dniPos;

    int pos2 = m_secondHash(oldKey);
    if (m_tabla[pos2].pos == DEFAULT_POSITION_VALUE) {
        m_tabla[pos2] = oldKey;
        return true;
    }

    m_rehash(oldKey, pos2);
    return true;
}

DniPos CuckooHashing::searchDNI(uint32_t dni) {
    int pos1 = m_firstHash({dni, DEFAULT_POSITION_VALUE});
    if (m_tabla[pos1].dni == dni) {
        return m_tabla[pos1];
    }

    int pos2 = m_secondHash({dni, DEFAULT_POSITION_VALUE});
    if (m_tabla[pos2].dni == dni) {
        return m_tabla[pos2];
    }

    return {0,0}; // Not found
}

bool CuckooHashing::deleteDNI(uint32_t dni) {
    DniPos dniPos = searchDNI(dni);
    if (dniPos.dni == DEFAULT_DNI_VALUE) {
        std::cerr << "[CuckooHashing] DNI not found\n";
        return false;
    }

    bool deleted = false;
    uint32_t pos1 = m_firstHash(dniPos);
    if (m_tabla[pos1].dni == dni) {
        m_tabla[pos1] = {DEFAULT_DNI_VALUE, DEFAULT_POSITION_VALUE};
        deleted = true;
    } else {
        uint32_t pos2 = m_secondHash(dniPos);
        if (m_tabla[pos2].dni == dni) {
            m_tabla[pos2] = {DEFAULT_DNI_VALUE, DEFAULT_POSITION_VALUE};
            deleted = true;
        } else {
            std::cerr << "[CuckooHashing] Error deleting DNI\n";
            return false;
        }
    }

    if (deleted) {
        uint32_t lastPosition = getlastPos();
        DatosPersona persona;
        const short PERSONA_DATA_SIZE = sizeof(char[9]) + sizeof(persona.nombres)  + sizeof(persona.apellidos)  +
                            sizeof(persona.nacimiento)  + sizeof(persona.nacionalidad)  +
                            sizeof(persona.direccion)  + sizeof(persona.lugarnacimiento) +
                            sizeof(char[10]) + sizeof(persona.correo) + sizeof(persona.estadoCivil);
        if ((lastPosition - PERSONA_DATA_SIZE) == dniPos.pos - 1) 
        {
        	std::cerr << "[CuckooHashing-deleted] El puntero de disco necesita ser reorganizado\n";
        	uint32_t pos = dniPos.pos - 1; // retrocede una persona
        	std::cerr << "[CuckooHashing-deleted] Nueva posicion de puntero: "<< pos << "bytes.\n";
            setlastPos(pos);
        }
    }

    if (getlastPos() < 1000000) {
        setlastPos(999999); // numero magico, no tocar
    }

    return true;
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
	const short DNI_SIZE = 9;
    const short PHONE_SIZE = 10;
    
    std::ifstream file("cuckohash.bin", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[CuckooHashing] Error opening file\n";
        return false;
    }
    
    DatosPersona persona;
    // Leer el tamaño de la tabla hash
    uint32_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));
    m_tabla.resize(size);
    
    const short PERSONA_DATA_SIZE = DNI_SIZE + sizeof(persona.nombres) + sizeof(persona.apellidos) +
                                    sizeof(persona.nacimiento) + sizeof(persona.nacionalidad) +
                                    sizeof(persona.direccion) + sizeof(persona.lugarnacimiento) +
                                    PHONE_SIZE + sizeof(persona.correo) + sizeof(persona.estadoCivil);

    // Leer cada elemento de la tabla hash
    for (DniPos& item : m_tabla) {
        if (!readHashItem(file, item)) {
            std::cerr << "[CuckooHashing] Error reading hash item\n";
            file.close();
            return false;
        }
        
         if (getlastPos() < item.pos) {
    		uint32_t newLastPos = item.pos + (PERSONA_DATA_SIZE-1);
    		setlastPos(newLastPos);
    	}
    }
    
    file.close();
    std::cout<<"[CUCKOOHASHING-READFILE] Ultima posicion escrita fue en el byte: " << getlastPos() << "\n";
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

// NO USAR
void CuckooHashing::printVector(uint32_t max){
	int i = 0;
	for(DniPos item : m_tabla){
		if(i > max){return;}
		std::cout<<"DNI: "<< item.dni << " POS:" << item.pos << std::endl;
		i++;
	}
}
