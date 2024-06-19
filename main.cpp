#include <iostream>
#include <fstream>
#include <sstream>
#include "models/btree.h"
#include "models/disk-manager.h"
#include "models/cuckohashing.h"
#include "models/firstload.h"
#include "models/dni-pos.h"
#include "models/personamanager.h"


// FOR API
#include "utility"
#include "string"
#include "crow.h"

// PERSON STRUCT
#include "models/personamodelo.h"

// CONSTANTS
#define INITIAL_TABLE_SIZE 1000000


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

void ReadPerson(uint32_t pos) {
   
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

// Helper function to copy strings safely
void safeStrCopy(char *dest, const std::string &src, size_t destSize)
{
    strncpy(dest, src.c_str(), destSize - 1);
    dest[destSize - 1] = '\0'; // Ensure null-termination
}

DatosPersona parseBody(const auto &body)
{
    DatosPersona datosPersona;

    try
    {
        datosPersona.dni = body["dni"].u();
        datosPersona.telefono = body["telefono"].u();

        safeStrCopy(datosPersona.nombres, body["nombres"].s(), sizeof(datosPersona.nombres));
        safeStrCopy(datosPersona.apellidos, body["apellidos"].s(), sizeof(datosPersona.apellidos));
        safeStrCopy(datosPersona.direccion, body["direccion"].s(), sizeof(datosPersona.direccion));
        safeStrCopy(datosPersona.nacimiento, body["nacimiento"].s(), sizeof(datosPersona.nacimiento));
        safeStrCopy(datosPersona.nacionalidad, body["nacionalidad"].s(), sizeof(datosPersona.nacionalidad));
        safeStrCopy(datosPersona.lugarnacimiento, body["lugarNacimiento"].s(), sizeof(datosPersona.lugarnacimiento));
        safeStrCopy(datosPersona.correo, body["correo"].s(), sizeof(datosPersona.correo));
        safeStrCopy(datosPersona.estadoCivil, body["estadoCivil"].s(), sizeof(datosPersona.estadoCivil));
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error(std::string("Error parsing body: ") + e.what());
    }

    return datosPersona;
}

bool isValidInteger(const std::string& str) {
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return !str.empty();  
}

// Función para pedir y validar el DNI del usuario
uint32_t askDNI() {
    std::string input;
    uint32_t dni;

    while (true) {
        std::cout << "Ingrese su DNI: ";
        std::cin >> input;
        if (isValidInteger(input)) {
            dni = std::stoul(input);
            break;
        } else {
            std::cout << "Entrada inválida. El DNI debe ser un número entero positivo.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    return dni;
}

void printMenu() {
    std::cout << "\nMenu TEST:\n";
    std::cout << "1. Read Person\n";
    std::cout << "2. Write Person\n";
    std::cout << "3. Delete Person\n";
    std::cout << "4. Exit TEST\n";
    std::cout << "Enter your choice: ";
}

int main()
{
	//TODO Cambiar esto para que no sea una aberracion en el main (talvez una clase)
    CuckooHashing* cuckoo = new CuckooHashing(INITIAL_TABLE_SIZE);
    PersonaManager personaManager(cuckoo);
    // comprobamos si el .bin existe
    std::ifstream file("cuckohash.bin", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[MAIN-LOG] cuckohash.bin no existe, buscando personas.txt\n";
        LoadCuckoo load;
        load.firstWrite(cuckoo,"personas.txt");
        std::cout<<"[MAIN-LOG] Exito, generando el archivo"<<std::endl;
    }  else {
        std::cout<<"[MAIN-LOG] Cargando tablaHash desde archivo cuckohash.bin"<<std::endl;
        if(cuckoo->readFile()){
        	std::cout<<"[MAIN-LOG] Vector online"<<std::endl;
  			//cuckoo->printVector(100);
        } else {
        	std::cout<<"[MAIN-ERROR] Hubo un error al leer el archivo"<<std::endl;
        }
    }
  	
  	int choice;
    do {
        printMenu();
        std::cin >> choice;
        std::cin.ignore(); // Ignore newline character left in the buffer

        switch (choice) {
            case 1: {
               	uint32_t dni = askDNI();
    			DniPos dniPos = cuckoo->searchDNI(dni);
			
    			std::cout << "[MAIN-LOG] Buscando DNI: " << dniPos.dni << " y POS: " << dniPos.pos << std::endl;
    			
    			if (dniPos.dni == 0) {
        			std::cout << "[MAIN-LOG] DNI no encontrado" << std::endl;
    			} else {
        			personaManager.readPerson(dniPos.pos);  
    			}
                break;
            }
            case 2: {
                DatosPersona persona;
                persona.dni = 12345678;
				persona.telefono = 945326723;
				
				std::strncpy(persona.nombres, "Pedro Miguel", sizeof(persona.nombres) - 1);
                persona.nombres[sizeof(persona.nombres) - 1] = '\0';

                std::strncpy(persona.apellidos, "Hernández Martínez", sizeof(persona.apellidos) - 1);
                persona.apellidos[sizeof(persona.apellidos) - 1] = '\0';

                std::strncpy(persona.direccion, "Pucallpa, Manantay", sizeof(persona.direccion) - 1);
                persona.direccion[sizeof(persona.direccion) - 1] = '\0';

                std::strncpy(persona.nacimiento, "20/9/78", sizeof(persona.nacimiento) - 1);
                persona.nacimiento[sizeof(persona.nacimiento) - 1] = '\0';

                std::strncpy(persona.nacionalidad, "EXT", sizeof(persona.nacionalidad) - 1);
                persona.nacionalidad[sizeof(persona.nacionalidad) - 1] = '\0';

                std::strncpy(persona.lugarnacimiento, "Barcelona", sizeof(persona.lugarnacimiento) - 1);
                persona.lugarnacimiento[sizeof(persona.lugarnacimiento) - 1] = '\0';

                std::strncpy(persona.correo, "luis7@gmail.com", sizeof(persona.correo) - 1);
                persona.correo[sizeof(persona.correo) - 1] = '\0';

                std::strncpy(persona.estadoCivil, "S", sizeof(persona.estadoCivil) - 1);
                persona.estadoCivil[sizeof(persona.estadoCivil) - 1] = '\0';

                if (personaManager.writePerson(persona)) {
                    std::cout << "Person written successfully.\n";
                } else {
                    std::cout << "Error writing person.\n";
                }
                break;
            }
            case 3: {
                uint32_t dni;
                std::cout << "Enter DNI to delete: ";
                std::cin >> dni;
                if (personaManager.deletePerson(dni)) {
                    std::cout << "Person deleted successfully.\n";
                } else {
                    std::cout << "Error deleting person.\n";
                }
                break;
            }
            case 4:
                std::cout << "Exiting TEST...\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 4);

    

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
                "s"
            };

            DniPos dniPos = cuckoo->searchDNI(dni);
            return crow::response(200, crow::json::wvalue{
                {"nombres", datosPersona.nombres},
                {"apellidos", datosPersona.apellidos},
                {"direccion", datosPersona.direccion},
                {"nacimiento", datosPersona.nacimiento},
                {"nacionalidad", datosPersona.nacionalidad},
                {"lugarNacimiento", datosPersona.lugarnacimiento},
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

    // PUT DNI ROUTE
    // THIS ROUTE WILL RECEIVE A JSON WITH A DNI AND WILL CREATE A PERSON IN THE HARD DRIVE
    CROW_ROUTE(app, "/api/person").methods(crow::HTTPMethod::POST)([&](const crow::request &req)
                                                                   {
        auto body = crow::json::load(req.body);
        if (!body)
        {
            return crow::response(400, "Invalid body");
        }

        uint32_t dni;
        try
        {
            DatosPersona datosPersona = parseBody(body);

            return crow::response(200, crow::json::wvalue{
                                           {"nombres", datosPersona.nombres},
                                           {"apellidos", datosPersona.apellidos},
                                           {"direccion", datosPersona.direccion},
                                           {"nacimiento", datosPersona.nacimiento},
                                           {"nacionalidad", datosPersona.nacionalidad},
                                           {"lugarNacimiento", datosPersona.lugarnacimiento},
                                           {"telefono", datosPersona.telefono},
                                           {"correo", datosPersona.correo},
                                           {"estadoCivil", datosPersona.estadoCivil},
                                           {"dni", datosPersona.dni},
                                       });
        }
        catch (const std::exception &err)
        {
            return crow::response(400, "Error");
        } });

    app.port(3000)
        .multithreaded()
        .run();

    return 0;
}
