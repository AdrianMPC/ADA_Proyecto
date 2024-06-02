#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>  
#include "../models/personamodelo.h"
#include "../models/cuckohashing.h"

// abrir archivo txt con todas las personas
// las personas están alamcenadas personas.txt
// cada persona está deivida por un enter
// función para leer los datos del archivo y convertirlos en objetos DatosPersona

std::vector<DatosPersona> readPeopleData(std::string filename) {
    std::vector<DatosPersona> personas;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file\n";
        return personas;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        DatosPersona persona;

        // leer y asignar cada campo
        // leer Dni
        std::getline(iss, token, ';');
        persona.dni = std::stoul(token);

        // leer nombres
        std::getline(iss, token, ';');
        std::strncpy(persona.nombres, token.c_str(), sizeof(persona.nombres) - 1);
        persona.nombres[sizeof(persona.nombres) - 1] = '\0';

        // leer apellidos
        std::getline(iss, token, ';');
        strncpy(persona.apellidos, token.c_str(), sizeof(persona.apellidos) - 1);
        persona.apellidos[sizeof(persona.apellidos) - 1] = '\0';

        // leer fecha de nacimiento
        std::getline(iss, token, ';');
        strncpy(persona.nacimiento, token.c_str(), sizeof(persona.nacimiento) - 1);
        persona.nacimiento[sizeof(persona.nacimiento) - 1] = '\0';

        // leer nacionalidad
        std::getline(iss, token, ';');
        strncpy(persona.nacionalidad, token.c_str(), sizeof(persona.nacionalidad) - 1);
        persona.nacionalidad[sizeof(persona.nacionalidad) - 1] = '\0';

        // leer lugar de nacimiento
        std::getline(iss, token, ';');
        strncpy(persona.lugarnacimiento, token.c_str(), sizeof(persona.lugarnacimiento) - 1);
        persona.lugarnacimiento[sizeof(persona.lugarnacimiento) - 1] = '\0';

        // leer direccion
        std::getline(iss, token, ';');
        strncpy(persona.direccion, token.c_str(), sizeof(persona.direccion) - 1);
        persona.direccion[sizeof(persona.direccion) - 1] = '\0';

        std::getline(iss, token, ';');
        persona.telefono = std::stoul(token);

        // correo
        std::getline(iss, token, ';');
        std::strncpy(persona.correo, token.c_str(), sizeof(persona.correo) - 1);
        persona.correo[sizeof(persona.correo) - 1] = '\0';

        // leer estado civil
        std::getline(iss, token, ';');
        persona.estadoCivil = token[0];

        // Agregar el objeto persona al vector
        personas.push_back(persona);
    }

    return personas;
}

template <typename T>
CuckooHashing writeCuckoo(T personas) {
    // Create cuckoo hash table with an initial size of 10000
    CuckooHashing cuckooHash(10000);

    // Positions in disk
    uint32_t pos = 1024;

    for (const auto& persona : personas) {
        DniPos dniPos(persona.dni, pos);
        cuckooHash.insertDni(dniPos);
        pos += sizeof(persona); // Increment position, assuming fixed size for simplicity
    }

    std::cout << "Personas guardadas" << std::endl;

    cuckooHash.writeFile();

    std::cout << "Personas guardadas en disco" << std::endl;

    cuckooHash.readFile();

    std::cout << "Personas leidas en disco" << std::endl;

    return cuckooHash;
}


int main() {
    // Get people
    std::vector<DatosPersona> personas = readPeopleData("personas.txt");

    CuckooHashing cuckoo = writeCuckoo(personas);

    return 0;
}
/*
int main() {
    // Leer los datos y obtener el vector de DatosPersona
    std::vector<DatosPersona> personas = leerDatos("personas.txt");

    // Mostrar los datos leídos (opcional)
    for (const auto& persona : personas) {
        std::cout << "DNI: " << persona.dni << "\n";
        std::cout << "Nombres: " << persona.nombres << "\n";
        std::cout << "Apellidos: " << persona.apellidos << "\n";
        std::cout << "Fecha de nacimiento: " << persona.nacimiento << "\n";
        std::cout << "Nacionalidad: " << persona.nacionalidad << "\n";
        std::cout << "Dirección: " << persona.direccion << "\n";
        std::cout << "Estado civil: " << persona.estadoCivil << "\n\n";
    }

    return 0;
}
*/