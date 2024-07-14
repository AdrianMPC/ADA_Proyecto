#include "../models/disk-manager.h"
#include "../models/personamanager.h"
#include <string>
#include <cstring>
#include <cstdlib>

PersonaManager::PersonaManager(CuckooHashing* cuckoo){
	m_cuckoo = cuckoo;
}

DatosPersona PersonaManager::readPerson(uint64_t pos){
	const short DNI_SIZE = 9;
    const short PHONE_SIZE = 10;

    DatosPersona persona;
    char _dni[DNI_SIZE];
    char num[PHONE_SIZE];

    DiskManager *diskM = DiskManager::getInstance();
    uint64_t posDisco = pos;

    auto readField = [&](const char* fieldName, char* field, short size) {
        diskM->readDisk(posDisco, field, size);
        posDisco += size + 1;
        //std::cout << fieldName << ": " << field << "\n";
    };

    std::cout << "Datos de las personas: \n";
    readField("DNI", _dni, DNI_SIZE);
    readField("Nombres", persona.nombres, sizeof(persona.nombres));
    readField("Apellidos", persona.apellidos, sizeof(persona.apellidos));
    readField("Nacimiento", persona.nacimiento, sizeof(persona.nacimiento));
    readField("Nacionalidad", persona.nacionalidad, sizeof(persona.nacionalidad));
    readField("Direccion", persona.direccion, sizeof(persona.direccion));
    readField("Lugar Nacimiento", persona.lugarnacimiento, sizeof(persona.lugarnacimiento));
    readField("Telefono", num, PHONE_SIZE);
    readField("Correo", persona.correo, sizeof(persona.correo));
    readField("Estado Civil", persona.estadoCivil, sizeof(persona.estadoCivil));
    persona.dni = std::atoi(_dni);
    persona.telefono = std::atoi(num);

    std::cout << "\n";
    return persona;
}

bool PersonaManager::writePerson(DatosPersona& persona){
	const short DNI_SIZE = 9;
    const short PHONE_SIZE = 10;
    
    DiskManager* diskM = DiskManager::getInstance();
    uint64_t punteroDisco = m_cuckoo->getlastPos();
    punteroDisco += 1;   // necesario, no borrar

    char _dni[DNI_SIZE];
    char _num[PHONE_SIZE];

    snprintf(_dni, sizeof(_dni), "%08u", persona.dni);
    snprintf(_num, sizeof(_num), "%08u", persona.telefono);
    
    DniPos dnipos;
    dnipos.dni = persona.dni;
    dnipos.pos = punteroDisco;

    if (m_cuckoo->insertDni(dnipos)) {
        diskM->borrarDatosNull(punteroDisco, sizeof(_dni));
        diskM->writeDisk(punteroDisco, _dni);
        punteroDisco += sizeof(_dni) + 1;

        diskM->borrarDatosNull(punteroDisco, sizeof(persona.nombres));
        diskM->writeDisk(punteroDisco, persona.nombres);
        punteroDisco += sizeof(persona.nombres) + 1;

        diskM->borrarDatosNull(punteroDisco, sizeof(persona.apellidos));
        diskM->writeDisk(punteroDisco, persona.apellidos);
        punteroDisco += sizeof(persona.apellidos) + 1;

        diskM->borrarDatosNull(punteroDisco, sizeof(persona.nacimiento));
        diskM->writeDisk(punteroDisco, persona.nacimiento);
        punteroDisco += sizeof(persona.nacimiento) + 1;

        diskM->borrarDatosNull(punteroDisco, sizeof(persona.nacionalidad));
        diskM->writeDisk(punteroDisco, persona.nacionalidad);
        punteroDisco += sizeof(persona.nacionalidad) + 1;

        diskM->borrarDatosNull(punteroDisco, sizeof(persona.direccion));
        diskM->writeDisk(punteroDisco, persona.direccion);
        punteroDisco += sizeof(persona.direccion) + 1;

        diskM->borrarDatosNull(punteroDisco, sizeof(persona.lugarnacimiento));
        diskM->writeDisk(punteroDisco, persona.lugarnacimiento);
        punteroDisco += sizeof(persona.lugarnacimiento) + 1;

        diskM->borrarDatosNull(punteroDisco, sizeof(_num));
        diskM->writeDisk(punteroDisco, _num);
        punteroDisco += sizeof(_num) + 1;

        diskM->borrarDatosNull(punteroDisco, sizeof(persona.correo));
        diskM->writeDisk(punteroDisco, persona.correo);
        punteroDisco += sizeof(persona.correo) + 1;

        diskM->borrarDatosNull(punteroDisco, sizeof(persona.estadoCivil));
        diskM->writeDisk(punteroDisco, persona.estadoCivil);
        punteroDisco += sizeof(persona.estadoCivil); // no +1, quiero que getlastpos de la ultima posicion escrita, no lista para escribir

        m_cuckoo->setlastPos(punteroDisco);

        std::cout << "[PersonaManager] Persona con DNI " << _dni << " se escribio" << " en posicion: "<< m_cuckoo->getlastPos() << "\n" ;
        return true;
    } else {
        std::cerr << "[PersonaManager] No se pudo insertar to insert DNI " << _dni << " into the hash table.\n";
        return false;
    }
}

bool PersonaManager::deletePerson(uint32_t dni){
    DniPos dniPos = m_cuckoo->searchDNI(dni);

    if (dniPos.dni != m_cuckoo->DEFAULT_DNI_VALUE) {
        bool result = m_cuckoo->deleteDNI(dni);
        if (result) {
            std::cout << "[PersonaManager] Persona con DNI " << dni << " eliminado correctamente.\n";
            std::cout << "[PersonaManager] Guardando cambios\n";        
        	return true;
        } else {
            std::cerr << "[PersonaManager] No se elimino la persona correctamente con " << dni << ".\n";
            return false;
        }
    } else {
        std::cerr << "[PersonaManager] Persona con DNI " << dni << " no fue encontrada.\n";
        return false;
    }
}
