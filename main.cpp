#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include "btree.h"

// AQUI ESA LA IMPLEMENTACION DEL BTREE TEMPORAL

/*
DatosPersona* parseLine(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    DatosPersona* person = new DatosPersona();

  // leer y asignar cada campo
    // leer Dni
    std::getline(ss, token, ';');
    person->dni = std::stoul(token);

    // leer nombres
    std::getline(ss, token, ';');
    std::strncpy(person->nombres, token.c_str(), sizeof(person->nombres) - 1);
    person->nombres[sizeof(person->nombres) - 1] = '\0';

    // leer apellidos
    std::getline(ss, token, ';');
    strncpy(person->apellidos, token.c_str(), sizeof(person->apellidos) - 1);
    person->apellidos[sizeof(person->apellidos) - 1] = '\0';

    // leer fecha de nacimiento
    std::getline(ss, token, ';');
    strncpy(person->nacimiento, token.c_str(), sizeof(person->nacimiento) - 1);
    person->nacimiento[sizeof(person->nacimiento) - 1] = '\0';

    // leer nacionalidad
    std::getline(ss, token, ';');
    strncpy(person->nacionalidad, token.c_str(), sizeof(person->nacionalidad) - 1);
    person->nacionalidad[sizeof(person->nacionalidad) - 1] = '\0';

    // leer lugar de nacimiento
    std::getline(ss, token, ';');
    strncpy(person->lugarnacimiento, token.c_str(), sizeof(person->lugarnacimiento) - 1);
    person->lugarnacimiento[sizeof(person->lugarnacimiento) - 1] = '\0';

    // leer direccion
    std::getline(ss, token, ';');
    strncpy(person->direccion, token.c_str(), sizeof(person->direccion) - 1);
    person->direccion[sizeof(person->direccion) - 1] = '\0';

    std::getline(ss, token, ';');
    person->telefono = std::stoul(token);

    // correo
    std::getline(ss, token, ';');
    std::strncpy(person->correo, token.c_str(), sizeof(person->correo) - 1);
    person->correo[sizeof(person->correo) - 1] = '\0';

    // leer estado civil
    std::getline(ss, token, ';');
    person->estadoCivil = token[0];
    return person;
}

void loadDataAndTest(BTree& tree, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    std::string line;
    //uint32_t filecount = 0;
    while (std::getline(file, line)) {
        DatosPersona* person = parseLine(line);
        tree.insertPerson(person);
       // filecount++;
        //std::cout << filecount << std::endl;
        //system("cls");
    }

    file.close();
}

int main() {
    BTree tree(3); 
    std::string file = "../personas_t.txt";
    loadDataAndTest(tree, file);

    std::cout << "Datos cargados" << std::endl;
    system("pause");
    uint32_t dniToSearch = 26854037; 
    std::cout << "Buscando DNI :" << dniToSearch << std::endl;

    DatosPersona* foundPerson = tree.searchDNI(dniToSearch);
    if (foundPerson) {
        std::cout << "Found person with DNI: " << foundPerson->dni << std::endl;
        std::cout << "Nombres: " << foundPerson->nombres << "\n";
        std::cout << "Apellidos: " << foundPerson->apellidos << "\n";
        std::cout << "Direccion: " << foundPerson->direccion << "\n";
        std::cout << "Fecha de nacimiento: " << foundPerson->nacimiento << "\n";
        std::cout << "Lugar de nacimiento: " << foundPerson->lugarnacimiento << "\n";
        std::cout << "Telefono: " << foundPerson->telefono << "\n";
        std::cout << "Nacionalidad: " << foundPerson->nacionalidad << "\n";
        std::cout << "Correo: " << foundPerson->correo << "\n";
        std::cout << "Estado civil: " << foundPerson->estadoCivil << "\n\n";
    } else {
        std::cout << "Person with DNI " << dniToSearch << " not found." << std::endl;
    }
    // Probar la eliminación
    uint32_t dniToDelete = 26854037; // DNI de ejemplo
    tree.deleteDNI(dniToDelete);
    foundPerson = tree.searchDNI(dniToDelete);
    std::cout << "Eliminando DNI :" << dniToDelete << std::endl;
    if (foundPerson) {
        std::cout << "Found person with DNI: " << foundPerson->dni << std::endl;
        std::cout << "Nombres: " << foundPerson->nombres << "\n";
        std::cout << "Apellidos: " << foundPerson->apellidos << "\n";
        std::cout << "Direccion: " << foundPerson->direccion << "\n";
        std::cout << "Fecha de nacimiento: " << foundPerson->nacimiento << "\n";
        std::cout << "Lugar de nacimiento: " << foundPerson->lugarnacimiento << "\n";
        std::cout << "Telefono: " << foundPerson->telefono << "\n";
        std::cout << "Nacionalidad: " << foundPerson->nacionalidad << "\n";
        std::cout << "Correo: " << foundPerson->correo << "\n";
        std::cout << "Estado civil: " << foundPerson->estadoCivil << "\n\n";
    } else {
        std::cout << "Person with DNI " << dniToSearch << " not found." << std::endl;
    }
    system("pause");
    return 0;
}
*/

int main(){
    return 0;
}