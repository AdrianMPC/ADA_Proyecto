                                                                                                                                                                                                                                                                                                                                                                                   #include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include "../models/disk-manager.h"
#include "../models/personamodelo.h"
#include "../models/cuckohashing.h"
#include "../models/dni-pos.h"

class LoadCuckoo {
public:
    LoadCuckoo() = default;
    
    void removeFirstSpace(std::string& str) {
    	auto pos = str.find(' ');
    	if (pos != std::string::npos) {
        	str.erase(pos, 1);
    	}
	}
	
    void firstWrite(CuckooHashing* cuck, std::string filename) {
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "[LoadCuckoo] personas.txt no existe, provee personas.txt en la carpeta build\n";
            exit(-1);
        }

        DiskManager* diskM = DiskManager::getInstance();
        uint32_t punteroDisco = 999997;
        uint32_t archivosEscritos = 0;

        std::string line;
        std::cerr << "[LoadCuckoo] personas.txt encontrado, cargando archivos\n";

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string token;
            DatosPersona persona;

            char _dni[9];
            punteroDisco += sizeof(persona.estadoCivil) + 1;

            std::getline(iss, token, ';');
            persona.dni = std::stoul(token); // data para hashear
            std::strncpy(_dni, token.c_str(), sizeof(_dni) - 1);
            _dni[sizeof(_dni) - 1] = '\0';

            DniPos dnipos;
            dnipos.dni = persona.dni;
            dnipos.pos = punteroDisco;

            if (cuck->insertDni(dnipos)) {
                diskM->borrarDatosNull(punteroDisco, sizeof(_dni));
                diskM->writeDisk(punteroDisco, _dni);
                
                punteroDisco += sizeof(_dni) + 1;

            	std::getline(iss, token, ';');
            	std::strncpy(persona.nombres, token.c_str(), sizeof(persona.nombres) - 1);
            	persona.nombres[sizeof(persona.nombres) - 1] = '\0';
            	diskM->borrarDatosNull(punteroDisco, sizeof(persona.nombres));
            	diskM->writeDisk(punteroDisco, persona.nombres);
            	punteroDisco += sizeof(persona.nombres) + 1;
	
            	std::getline(iss, token, ';');
            	std::strncpy(persona.apellidos, token.c_str(), sizeof(persona.apellidos) - 1);
            	persona.apellidos[sizeof(persona.apellidos) - 1] = '\0';
            	diskM->borrarDatosNull(punteroDisco, sizeof(persona.apellidos));
            	diskM->writeDisk(punteroDisco, persona.apellidos);
            	punteroDisco += sizeof(persona.apellidos) + 1;
	
            	std::getline(iss, token, ';');
            	std::strncpy(persona.nacimiento, token.c_str(), sizeof(persona.nacimiento) - 1);
            	persona.nacimiento[sizeof(persona.nacimiento) - 1] = '\0';
            	diskM->borrarDatosNull(punteroDisco, sizeof(persona.nacimiento));
            	diskM->writeDisk(punteroDisco, persona.nacimiento);
            	punteroDisco += sizeof(persona.nacimiento) + 1;
	
            	std::getline(iss, token, ';');
            	std::strncpy(persona.nacionalidad, token.c_str(), sizeof(persona.nacionalidad) - 1);
            	persona.nacionalidad[sizeof(persona.nacionalidad) - 1] = '\0';
            	diskM->borrarDatosNull(punteroDisco, sizeof(persona.nacionalidad));
            	diskM->writeDisk(punteroDisco, persona.nacionalidad);
            	punteroDisco += sizeof(persona.nacionalidad) + 1;
	
            	std::getline(iss, token, ';');
            	removeFirstSpace(token);
            	std::strncpy(persona.direccion, token.c_str(), sizeof(persona.direccion) - 1);
            	persona.direccion[sizeof(persona.direccion) - 1] = '\0';
            	diskM->borrarDatosNull(punteroDisco, sizeof(persona.direccion));
            	diskM->writeDisk(punteroDisco, persona.direccion);
            	punteroDisco += sizeof(persona.direccion) + 1;
	
            	std::getline(iss, token, ';');
            	removeFirstSpace(token);
            	std::strncpy(persona.lugarnacimiento, token.c_str(), sizeof(persona.lugarnacimiento) - 1);
            	persona.lugarnacimiento[sizeof(persona.lugarnacimiento) - 1] = '\0';
            	diskM->borrarDatosNull(punteroDisco, sizeof(persona.lugarnacimiento));
            	diskM->writeDisk(punteroDisco, persona.lugarnacimiento);
            	punteroDisco += sizeof(persona.lugarnacimiento) + 1;
	
            	char telefono[10];
            	std::getline(iss, token, ';');
            	std::strncpy(telefono, token.c_str(), sizeof(telefono) - 1);
            	telefono[sizeof(telefono) - 1] = '\0';
            	diskM->borrarDatosNull(punteroDisco, sizeof(telefono));
            	diskM->writeDisk(punteroDisco, telefono);
            	punteroDisco += sizeof(telefono) + 1;
	
            	std::getline(iss, token, ';');
            	removeFirstSpace(token);
            	std::strncpy(persona.correo, token.c_str(), sizeof(persona.correo) - 1);
            	persona.correo[sizeof(persona.correo) - 1] = '\0';
            	diskM->borrarDatosNull(punteroDisco, sizeof(persona.correo));
            	diskM->writeDisk(punteroDisco, persona.correo);
            	punteroDisco += sizeof(persona.correo) + 1;
	
            	std::getline(iss, token, ';');
            	std::strncpy(persona.estadoCivil, token.c_str(), sizeof(persona.estadoCivil) - 1);
            	persona.estadoCivil[sizeof(persona.estadoCivil) - 1] = '\0';
            	diskM->borrarDatosNull(punteroDisco, sizeof(persona.estadoCivil));
            	diskM->writeDisk(punteroDisco, persona.estadoCivil);
	
            	if (cuck->getlastPos() < punteroDisco) {
            		int64_t pos = punteroDisco + sizeof(persona.estadoCivil);
                	cuck->setlastPos(pos);
            	}
            	//archivosEscritos++;
	
            	//switch (archivosEscritos) {
                //	case 100:
                //	case 500:
//                	case 1000:
//                	case 10000:
                //	case 50000:
//                	case 100000:
                //	case 500000:
                //	case 1000000:
//                	case 5000000:
                //	case 10000000:
                //	case 20000000:
                //	case 33000000:
                 //   	std::cout << "[FIRSTLOAD] " << archivosEscritos << " archivos escritos\n";
               //     	break;
            //	}
            } else {
                std::cerr << "[FIRSTLOAD] No se pudo escribir el DNI: " << _dni << " en la posicion " << punteroDisco << " bytes" << std::endl;
            }   
        }
        std::cout<<"[FIRSTLOAD] Ultima posicion escrita fue en el byte: " << cuck->getlastPos() << "\n";
        std::cout << "[FIRSTLOAD] Se completo la escritura en disco, generando cuckoohash.bin\n";
        writeCuckoo(cuck);
    }

    bool writeCuckoo(CuckooHashing* cuckooHash) {
        if (cuckooHash->writeFile()) {
            std::cout << "[CuckooHashing] cuckoohash.bin se genero correctamente\n";
            return true;
        }
        return false;
    }
};




