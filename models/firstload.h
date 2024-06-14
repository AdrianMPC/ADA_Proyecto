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

#define INITIAL_TABLE_SIZE 40000000

class LoadCuckoo{
    public:
        LoadCuckoo() = default;
        void firstWrite(std::string filename)
        {
            //std::vector<DniPos> vectorHash;
            // vamos a empezar desde 1mb de offset
            std::ifstream file(filename);

            if (!file.is_open())
            {
                std::cerr << "[LoadCuckoo] personas.txt no existe, provee personas.txt enla carpeta build\n";
                exit(-1);
            }
            CuckooHashing* cuck = new CuckooHashing(INITIAL_TABLE_SIZE);
            DiskManager *diskM = DiskManager::getInstance();
            uint32_t punteroDisco = 999997;


            std::string line;
            while (std::getline(file, line))
            {
                std::istringstream iss(line);
                std::string token;
                DatosPersona persona;

                // leer y asignar cada campo
                // leer Dni
                
                char dni[9]; // data para escribir en disco

                // set puntero
                punteroDisco += sizeof(persona.estadoCivil) + 1; // 2+1 = 3 e.g 999997 + 3 = 1000000

                std::getline(iss, token, ';');
                persona.dni = std::stoul(token); // data para hashear
                std::strncpy(dni, token.c_str(), sizeof(persona.dni) - 1);
                dni[sizeof(dni) - 1] = '\0';

                // 1. Hasheamos
                DniPos dnipos = {
                    dni = dni;
                    pos = punteroDisco;
                };

                if(cuck->insertDni(dnipos)){ 
                    // 2. Escribimos en disco
                    diskM->writeDisk(punteroDisco,dni);
                } else {
                    std::cerr<<"[firstload] No se pudo escribir el DNI: "<< dni << " en la posicion " << punteroDisco << " bytes" << std::endl;
                }

                punteroDisco += sizeof(dni) + 1; // e.g 1000000 + 10 = 1000010
                // leer nombres, solo escribimos en disco
                std::getline(iss, token, ';');
                std::strncpy(persona.nombres, token.c_str(), sizeof(persona.nombres) - 1);
                persona.nombres[sizeof(persona.nombres) - 1] = '\0';

                diskM->writeDisk(punteroDisco,persona.nombres);
                punteroDisco += sizeof(persona.nombres) + 1; // e.g 1000010 + 41(+1) = 1000052

                // leer apellidos,  solo escribimos en disco
                std::getline(iss, token, ';');
                strncpy(persona.apellidos, token.c_str(), sizeof(persona.apellidos) - 1);
                persona.apellidos[sizeof(persona.apellidos) - 1] = '\0';

                diskM->writeDisk(punteroDisco,persona.apellidos);
                punteroDisco += sizeof(persona.apellidos) + 1; // e.g 1000052 + 41(+1) = 1000094

                //
                // leer direccion,  solo escribimos en disco
                std::getline(iss, token, ';');
                strncpy(persona.direccion, token.c_str(), sizeof(persona.direccion) - 1);
                persona.direccion[sizeof(persona.direccion) - 1] = '\0';

                diskM->writeDisk(punteroDisco,persona.direccion);
                punteroDisco += sizeof(persona.direccion) + 1; // e.g 1000094 + 31(+1) = 1000126

                //
                // leer nacimiento,  solo escribimos en disco
                std::getline(iss, token, ';');
                strncpy(persona.nacimiento, token.c_str(), sizeof(persona.nacimiento) - 1);
                persona.nacimiento[sizeof(persona.nacimiento) - 1] = '\0';

                diskM->writeDisk(punteroDisco,persona.nacimiento);
                punteroDisco += sizeof(persona.nacimiento) + 1; 

                //
                // leer nacionalidad,  solo escribimos en disco
                std::getline(iss, token, ';');
                strncpy(persona.nacionalidad, token.c_str(), sizeof(persona.nacionalidad) - 1);
                persona.nacionalidad[sizeof(persona.nacionalidad) - 1] = '\0';

                diskM->writeDisk(punteroDisco,persona.nacionalidad);
                punteroDisco += sizeof(persona.nacionalidad) + 1; 

                //
                // leer lugarnacimiento,  solo escribimos en disco
                std::getline(iss, token, ';');
                strncpy(persona.lugarnacimiento, token.c_str(), sizeof(persona.lugarnacimiento) - 1);
                persona.lugarnacimiento[sizeof(persona.lugarnacimiento) - 1] = '\0';

                diskM->writeDisk(punteroDisco,persona.lugarnacimiento);
                punteroDisco += sizeof(persona.lugarnacimiento) + 1; 

                //
                // leer telefono,  solo escribimos en disco
                char telefono[10];
                std::getline(iss, token, ';');
                strncpy(telefono, token.c_str(), sizeof(telefono) - 1);
                telefono[sizeof(telefono) - 1] = '\0';

                diskM->writeDisk(punteroDisco,telefono);
                punteroDisco += sizeof(telefono) + 1;

                //
                // leer correo,  solo escribimos en disco
                std::getline(iss, token, ';');
                strncpy(persona.correo, token.c_str(), sizeof(persona.correo) - 1);
                persona.correo[sizeof(persona.correo) - 1] = '\0';

                diskM->writeDisk(punteroDisco,persona.correo);
                punteroDisco += sizeof(persona.correo) + 1; 

                //
                // leer estadoCivil,  solo escribimos en disco
                std::getline(iss, token, ';');
                strncpy(persona.estadoCivil, token.c_str(), sizeof(persona.estadoCivil) - 1);
                persona.estadoCivil[sizeof(persona.estadoCivil) - 1] = '\0';

                diskM->writeDisk(punteroDisco,persona.estadoCivil);
            }
            writeCuckoo(cuck);
        }

    bool writeCuckoo(CuckooHashing* cuckooHash)
    {
        if(cuckooHash->writeFile()){
            std::cout<<"Se genero el file correctamente\n";
            return true;
        }
        return false;
    }
};

// abrir archivo txt con todas las personas
// las personas están alamcenadas personas.txt
// cada persona está deivida por un enter
// función para leer los datos del archivo y convertirlos en objetos DatosPersona



