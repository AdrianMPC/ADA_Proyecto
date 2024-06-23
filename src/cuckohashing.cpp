#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstddef>
#include "../models/cuckohashing.h"
#include "../models/personamodelo.h"
#include "../models/dni-pos.h"


CuckooHashing::CuckooHashing(uint32_t sizeTabla) : m_sizeTabla(sizeTabla) {
    m_tabla.resize(sizeTabla, {DEFAULT_DNI_VALUE, DEFAULT_POSITION_VALUE});
}

int32_t CuckooHashing::m_firstHash(DniPos dniPos) {
    return hashFunction(dniPos.dni, m_sizeTabla);
}

int32_t CuckooHashing::m_secondHash(DniPos dniPos) {
    return hashFunction(dniPos.dni * 37, m_sizeTabla); // Cambiar el multiplicador para la segunda función
}

uint32_t CuckooHashing::getSize() {
    return this->m_sizeTabla;
}

void CuckooHashing::m_rehash(DniPos dniPos, uint64_t pos) {
    uint32_t loopCount = 0;
    while (loopCount < m_sizeTabla) {
        std::swap(dniPos, m_tabla[pos]);
        uint32_t posCheck = m_firstHash(dniPos);   
        
        if(pos == posCheck){ // estamos en hash 1
        	posCheck = m_secondHash(dniPos);
        }
        
        if (m_tabla[posCheck].pos == DEFAULT_POSITION_VALUE) {
            m_tabla[posCheck] = dniPos;
            return;
        }
        
        pos = posCheck;
        loopCount++;
    }

    // If we end up here, we need to rehash the entire table
    std::cout << "Rehashing required!" << std::endl;
    std::cout << "[CuckooHashing] Rehash de DNI:  " << dniPos.dni << " y pos: " << dniPos.pos << "\n";
    m_rehashAll(dniPos);
    insertDni(dniPos); // Reinsert the key after rehashing
}

void CuckooHashing::m_rehashAll(DniPos dniPos) {
    std::vector<DniPos> oldTable = m_tabla;
	std::cout << "[CuckooHashing] Aumentando m_tabla size de " << m_sizeTabla;
    m_sizeTabla += 10000000;
    std::cout << " a " << m_sizeTabla << "\n";
    m_tabla.clear();
    m_tabla.resize(m_sizeTabla, {DEFAULT_DNI_VALUE, DEFAULT_POSITION_VALUE});

    for (DniPos key : oldTable) {
        if (key.pos != DEFAULT_POSITION_VALUE) {
            insertDni(key);
        }
    }
    oldTable.resize(DEFAULT_POSITION_VALUE);
}

bool CuckooHashing::insertDni(DniPos dniPos) {
    DniPos check = searchDNI(dniPos.dni);
    if(check.dni != DEFAULT_DNI_VALUE){std::cout<<"[CUCKOOHASH] DNI ya existente!\n"; return false;}
    uint32_t pos1 = m_firstHash(dniPos);
    if (m_tabla[pos1].pos == DEFAULT_POSITION_VALUE) {
        m_tabla[pos1] = dniPos;
        //std::cout << "[CUCKOOHASHING] Se inserto sin rehash el dni: "<<dniPos.dni<<"\n"; 
        return true;
    }

    DniPos oldKey = m_tabla[pos1];
    m_tabla[pos1] = dniPos;
    
    uint32_t posCheck = m_firstHash(oldKey);
    
    if( posCheck ==  pos1 ){  // si esta en el primer hash, haz hash2
    	posCheck = m_secondHash(oldKey);
    } 
    
    if (m_tabla[posCheck].pos == DEFAULT_POSITION_VALUE) {
        m_tabla[posCheck] = oldKey;
        //std::cout << "[CUCKOOHASHING] Se inserto sin rehash el dni2: "<<oldKey.dni<<"\n"; 
        return true;
    }

    m_rehash(oldKey, posCheck);
   //	std::cout << "[CUCKOOHASHING] Se inserto CON rehash: "<<oldKey.dni<<"\n"; 
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

    return {DEFAULT_DNI_VALUE,DEFAULT_POSITION_VALUE}; // Not found
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
        uint64_t lastPosition = getlastPos();
        DatosPersona persona;
        const short PERSONA_DATA_SIZE = sizeof(char[9]) + sizeof(persona.nombres)  + sizeof(persona.apellidos)  +
                            sizeof(persona.nacimiento)  + sizeof(persona.nacionalidad)  +
                            sizeof(persona.direccion)  + sizeof(persona.lugarnacimiento) +
                            sizeof(char[10]) + sizeof(persona.correo) + sizeof(persona.estadoCivil);
        if ((lastPosition - PERSONA_DATA_SIZE) == dniPos.pos - 1) 
        {
        	std::cerr << "[CuckooHashing-delete] El puntero de disco necesita ser reorganizado.\n";
        	uint64_t pos = dniPos.pos - 1; // retrocede una persona
        	std::cerr << "[CuckooHashing-delete] Nueva posicion de puntero: "<< pos << " bytes.\n";
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
    std::cout<<"\n[CuckooHashing-WriteFile] VectorSize: "<<size<<"\n";
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));
     if (!file) {
        std::cerr << "[CuckooHashing] Error writing size to file\n";
        return false;
    }

    // Write each element of the hash table
    for (const auto& item : m_tabla) {
        file.write(reinterpret_cast<const char*>(&item), sizeof(item));
        
        if (!file) {
            std::cerr << "[CuckooHashing] Error writing item to file\n";
            return false;
        }
    }

    file.close();
    if (!file) {
        std::cerr << "[CuckooHashing] Error closing the file\n";
        return false;
    }
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
    std::cout<<"[CUCKOOHASHING-READFILE] VECTORHASH size: " << m_tabla.size() << std::endl;
    
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
    		uint64_t newLastPos = item.pos + (PERSONA_DATA_SIZE-1);
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

uint64_t CuckooHashing::getlastPos(){
    return lastPos;
}

void CuckooHashing::setlastPos(uint64_t _newlastpos){
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
