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



class LoadCuckoo{
    public:
        LoadCuckoo() = default;
        void firstWrite(CuckooHashing* cuck, std::string filename)
        {
            //std::vector<DniPos> vectorHash;
            // vamos a empezar desde 1mb de offset
            std::cout<<"[FIRSTWRITE] cuckoo reference "<< cuck << std::endl;
            std::ifstream file(filename);

            if (!file.is_open())
            {
                std::cerr << "[LoadCuckoo] personas.txt no existe, provee personas.txt enla carpeta build\n";
                exit(-1);
            }
            DiskManager *diskM = DiskManager::getInstance();
            uint32_t punteroDisco = 999997;
			uint32_t archivosEscritos = 0;

            std::string line;
            std::cerr << "[LoadCuckoo] personas.txt encontrado, cargando archivos\n";
            while (std::getline(file, line))
            {
                std::istringstream iss(line);
                std::string token;
                DatosPersona persona;

                // leer y asignar cada campo
                // leer Dni
                
                char _dni[9]; // data para escribir en disco

                // set puntero
                punteroDisco += sizeof(persona.estadoCivil) + 1; // 2+1 = 3 e.g 999997 + 3 = 1000000
               // std::cout<<punteroDisco<<std::endl;

                std::getline(iss, token, ';');
                persona.dni = std::stoul(token); // data para hashear
                std::strncpy(_dni, token.c_str(), sizeof(_dni) - 1);
                _dni[sizeof(_dni) - 1] = '\0';

                // 1. Hasheamos
                DniPos dnipos;
                dnipos.dni = persona.dni;
                dnipos.pos = punteroDisco;
                
               // std::cout<<"[CUCKOOHASH] TEST DNIPOS\n"<< dnipos.dni << " - " << dnipos.pos << "\n";

                if(cuck->insertDni(dnipos)){ 
                    // 2. Escribimos en disco
                    diskM->borrarDatosNull(punteroDisco, sizeof(_dni));
                    diskM->writeDisk(punteroDisco,_dni);
                } else {
                    std::cerr<<"[FIRSTLOAD] No se pudo escribir el DNI: "<< _dni << " en la posicion " << punteroDisco << " bytes" << std::endl;
                }

                punteroDisco += sizeof(_dni) + 1; // e.g 1000000 + 10 = 1000010
               // std::cout<<punteroDisco<<std::endl;
                // leer nombres, solo escribimos en disco
                std::getline(iss, token, ';');
                std::strncpy(persona.nombres, token.c_str(), sizeof(persona.nombres) - 1);
                persona.nombres[sizeof(persona.nombres) - 1] = '\0';
                
				diskM->borrarDatosNull(punteroDisco, sizeof(persona.nombres));
                diskM->writeDisk(punteroDisco,persona.nombres);
                punteroDisco += sizeof(persona.nombres) + 1; // e.g 1000010 + 41(+1) = 1000052
				//std::cout<<punteroDisco<<std::endl;
                // leer apellidos,  solo escribimos en disco
                std::getline(iss, token, ';');
                strncpy(persona.apellidos, token.c_str(), sizeof(persona.apellidos) - 1);
                persona.apellidos[sizeof(persona.apellidos) - 1] = '\0';
                
				diskM->borrarDatosNull(punteroDisco, sizeof(persona.apellidos));
                diskM->writeDisk(punteroDisco,persona.apellidos);
                punteroDisco += sizeof(persona.apellidos) + 1; // e.g 1000052 + 41(+1) = 1000094
				//std::cout<<punteroDisco<<std::endl;
                //
                // leer direccion,  solo escribimos en disco
                std::getline(iss, token, ';');
                strncpy(persona.direccion, token.c_str(), sizeof(persona.direccion) - 1);
                persona.direccion[sizeof(persona.direccion) - 1] = '\0';
                
				diskM->borrarDatosNull(punteroDisco, sizeof(persona.direccion));
                diskM->writeDisk(punteroDisco,persona.direccion);
                punteroDisco += sizeof(persona.direccion) + 1; // e.g 1000094 + 31(+1) = 1000126
				//std::cout<<punteroDisco<<std::endl;
                //
                // leer nacimiento,  solo escribimos en disco
                std::getline(iss, token, ';');
                strncpy(persona.nacimiento, token.c_str(), sizeof(persona.nacimiento) - 1);
                persona.nacimiento[sizeof(persona.nacimiento) - 1] = '\0';
                
				diskM->borrarDatosNull(punteroDisco, sizeof(persona.nacimiento));
                diskM->writeDisk(punteroDisco,persona.nacimiento);
                punteroDisco += sizeof(persona.nacimiento) + 1; // e.g 10000126 + 9(+1) = 1000136
				//std::cout<<punteroDisco<<std::endl;
                //
                // leer nacionalidad,  solo escribimos en disco
                std::getline(iss, token, ';');
                strncpy(persona.nacionalidad, token.c_str(), sizeof(persona.nacionalidad) - 1);
                persona.nacionalidad[sizeof(persona.nacionalidad) - 1] = '\0';
				//std::cout<<"Nacionalidad. "<< sizeof(persona.nacionalidad) << std::endl;
				diskM->borrarDatosNull(punteroDisco, sizeof(persona.nacionalidad));
                diskM->writeDisk(punteroDisco,persona.nacionalidad);
                punteroDisco += sizeof(persona.nacionalidad) + 1;  // e.g 1000136 + 4(+1) = 1000141
				//std::cout<<punteroDisco<<std::endl;
                //
                // leer lugarnacimiento,  solo escribimos en disco
                std::getline(iss, token, ';');
                strncpy(persona.lugarnacimiento, token.c_str(), sizeof(persona.lugarnacimiento) - 1);
                persona.lugarnacimiento[sizeof(persona.lugarnacimiento) - 1] = '\0';
                
				diskM->borrarDatosNull(punteroDisco, sizeof(persona.lugarnacimiento));
                diskM->writeDisk(punteroDisco,persona.lugarnacimiento);
                punteroDisco += sizeof(persona.lugarnacimiento) + 1; 

                //
                // leer telefono,  solo escribimos en disco
                char telefono[10];
                std::getline(iss, token, ';');
                strncpy(telefono, token.c_str(), sizeof(telefono) - 1);
                telefono[sizeof(telefono) - 1] = '\0';
				
				diskM->borrarDatosNull(punteroDisco, sizeof(telefono));
                diskM->writeDisk(punteroDisco,telefono);
                punteroDisco += sizeof(telefono) + 1;

                //
                // leer correo,  solo escribimos en disco
                std::getline(iss, token, ';');
                strncpy(persona.correo, token.c_str(), sizeof(persona.correo) - 1);
                persona.correo[sizeof(persona.correo) - 1] = '\0';
                
				diskM->borrarDatosNull(punteroDisco, sizeof(persona.correo));
                diskM->writeDisk(punteroDisco,persona.correo);
                punteroDisco += sizeof(persona.correo) + 1; 

                //
                // leer estadoCivil,  solo escribimos en disco
                std::getline(iss, token, ';');
                strncpy(persona.estadoCivil, token.c_str(), sizeof(persona.estadoCivil) - 1);
                persona.estadoCivil[sizeof(persona.estadoCivil) - 1] = '\0';
				
				diskM->borrarDatosNull(punteroDisco, sizeof(persona.estadoCivil));
                diskM->writeDisk(punteroDisco,persona.estadoCivil);

                if(cuck->getlastPos() < punteroDisco){ // obtener el ultimo puntero
                    cuck->setlastPos(punteroDisco);
                }
                archivosEscritos++;
                
                switch(archivosEscritos){
                	case 100:
                		std::cout<<"[FIRSTLOAD] "<<archivosEscritos<< " archivos escritos\n";
                		break;
                	case 500:
                		std::cout<<"[FIRSTLOAD] "<<archivosEscritos<< " archivos escritos\n";
                		break;
                	case 1000:
                		std::cout<<"[FIRSTLOAD] "<<archivosEscritos<< " archivos escritos\n";
                		break;
                	case 10000:
                		std::cout<<"[FIRSTLOAD] "<<archivosEscritos<< " archivos escritos\n";
                		break;
                	case 50000:
                		std::cout<<"[FIRSTLOAD] "<<archivosEscritos<< " archivos escritos\n";
                		break;
                	case 100000:
                		std::cout<<"[FIRSTLOAD] "<<archivosEscritos<< " archivos escritos\n";
                		break;
                	case 500000:
                		std::cout<<"[FIRSTLOAD] "<<archivosEscritos<< " archivos escritos\n";
                		break;
                	case 1000000:
                		std::cout<<"[FIRSTLOAD] "<<archivosEscritos<< " archivos escritos\n";
                		break;
                	case 5000000:
                		std::cout<<"[FIRSTLOAD] "<<archivosEscritos<< " archivos escritos\n";
                		break;
                	case 10000000:
                		std::cout<<"[FIRSTLOAD] "<<archivosEscritos<< " archivos escritos\n";
                		break;
                	case 20000000:
                		std::cout<<"[FIRSTLOAD] "<<archivosEscritos<< " archivos escritos\n";
                		break;
                	case 33000000:
                		std::cout<<"[FIRSTLOAD] "<<archivosEscritos<< " archivos escritos\n";
                		break;
                }
                
            }
            std::cout<<"[FIRSTLOAD] Se completo la escritura en disco, generando cuckoohash.bin\n";
            std::cout<<"[FIRSTLOAD] Imprimir vector actual\n";
            cuck->printVector(100);
            writeCuckoo(cuck);    
        }

    bool writeCuckoo(CuckooHashing* cuckooHash)
    {
        if(cuckooHash->writeFile()){
            std::cout<<"[CuckooHashing] cuckoohash.bin se genero correctamente\n";
            return true;
        }
        return false;
    }
};

// abrir archivo txt con todas las personas
// las personas están alamcenadas personas.txt
// cada persona está deivida por un enter
// función para leer los datos del archivo y convertirlos en objetos DatosPersona



