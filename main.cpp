#include <iostream>
#include <fstream>
#include <sstream>
#include "models/btree.h"
#include "models/disk-manager.h"
#include "models/cuckohashing.h"
#include "dni-pos.h"

// FOR API
#include "utility"
#include "string"
#include "crow.h"

// PERSON STRUCT
#include "models/personamodelo.h"

uint32_t parseLine(const std::string &line)
{
    std::stringstream ss(line);
    std::string token;
    std::getline(ss, token, ';');
    return std::stoul(token);
}

void loadDataAndTest(BTree &tree, const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }
    std::string line;

    while (std::getline(file, line))
    {
        uint32_t person = parseLine(line);
        tree.insertPerson(person);
    }

    file.close();
}

void BTreeTesting()
{
    BTree tree(3);
    std::string file = "../personas.txt";
    loadDataAndTest(tree, file);
    std::cout << "Datos cargados...\n";
    std::cout << "Presiona cualquier tecla para continuar...\n";
    std::cin.get();

    uint32_t dniToSearch = 26854037;
    std::cout << "Buscando DNI: " << dniToSearch << "\n";

    if (tree.searchDNI(dniToSearch))
    {
        std::cout << "SI se encontró el DNI #" << dniToSearch << "\n";
    }
    else
    {
        std::cout << "NO se encontró el DNI #" << dniToSearch << "\n";
    }

    tree.deleteDNI(dniToSearch);

    if (tree.searchDNI(dniToSearch))
    {
        std::cout << "SI se encontró el DNI #" << dniToSearch << "\n";
    }
    else
    {
        std::cout << "NO se encontró el DNI #" << dniToSearch << "\n";
    }
}

// LA FORMULA PARA DETERMINAR QUE POSICION USAR SIGUIENTE ES:
// (TAMAÑO DE DATOS (QUE YA INCLUYE EL \0) + 1)
// IMPORTANTE, HACER DESDE 1'000'000 = 1MB para no corromper nada de la particion
void WriteDiskTest()
{
    DiskManager *diskM = DiskManager::getInstance();
    int dataso = 70799417;
    char dataso_leer[9];
    char test[10] = "hola12345"; // char de 9 + 1 el null
    char test_leer[10];
    diskM->borrarDatosNull(1000000, sizeof(dataso_leer));
    diskM->writeDiskInt(1000000, dataso);
    diskM->borrarDatosNull(1000010, sizeof(test));
    diskM->writeDisk(1000010, test);
    diskM->readDisk(1000000, dataso_leer, sizeof(dataso_leer));
    diskM->readDisk(1000010, test_leer, sizeof(test_leer));
    std::cout << dataso_leer << "\n";
    std::cout << test_leer << "\n";
}

void writeDiskAndCuckoo(uint32_t dni)
{
    DiskManager *diskM = DiskManager::getInstance();
    int dataso = 70799417;

    // diskM->writeDisk(1000010,test);
}

int main()
{
    CuckooHashing cuckoo(10);

    for (int i = 0; i < 200; i++)
    {
        DniPos dniPos(i, i + 84);
        cuckoo.insertDni(dniPos);
    }

    cuckoo.insertDni(88888888);

    std::cout << "Testing";
    DniPos dniPos = cuckoo.searchDNI(20);
    std::cout << dniPos.dni << ":" << dniPos.pos << std::endl;

    // API WITH CROW
    crow::SimpleApp app;

    // Redis client deleted

    CROW_ROUTE(app, "/health")
    ([]()
     { return "Working fine..."; });

    // GET DNI ROUTE
    // THIS ROUTE WILL RECEIVE A JSON WITH A DNI AND WILL RETURN THE PERSON DATA
    // IT WILL USE THE CUCKOO HASHING TO SEARCH THE DNI
    // AND THEN IT WILL FIND IN THE HARD DRIVE THE PERSON DATA
    CROW_ROUTE(app, "/api/person").methods(crow::HTTPMethod::GET)([&](const crow::request &req)
                                                                  {
        auto dniParam = req.url_params.get("dni");
        if (!dniParam)
        {
            return crow::response(400, "Invalid body");
        }
        
        uint32_t dni;
        try
        {
            dni = std::stoul(dniParam);
        }
        catch (const std::exception &err)
        {
            return crow::response(400, "Invalid DNI");
        }

        try
        {
            DatosPersona datosPersona{
                dni,
                "Juan",
                "Perez",
                "Calle falsa 123",
                "02/11/23",
                "PER",
                "Lima",
                99999999,
                "andre@gmail.com",
                'S'
            };

            DniPos dniPos = cuckoo.searchDNI(dni);
            return crow::response(200, crow::json::wvalue{
                {"nombres", datosPersona.nombres},
                {"apellidos", datosPersona.apellidos},
                {"direccion", datosPersona.direccion},
                {"nacimiento", datosPersona.nacimiento},
                {"nacionalidad", datosPersona.nacionalidad},
                {"lugarnacimiento", datosPersona.lugarnacimiento},
                {"telefono", datosPersona.telefono},
                {"correo", datosPersona.correo},
                {"estadoCivil", datosPersona.estadoCivil},
                {"dni", dniPos.dni},
             }
                );
        }
        catch (const std::runtime_error &err)
        {
            return crow::response(404, "DNI Not Found");
        } });

    app.port(3000).multithreaded().run();

    return 0;
}
