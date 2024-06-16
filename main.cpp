#include <iostream>
#include <fstream>
#include <sstream>
#include "models/btree.h"
#include "models/disk-manager.h"
#include "models/cuckohashing.h"
#include "models/firstload.h"
#include "dni-pos.h"


// FOR API
#include "utility"
#include "string"
#include "crow.h"

// PERSON STRUCT
#include "models/personamodelo.h"

// CONSTANTS
#define INITIAL_TABLE_SIZE 40000000


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

void ReadPerson(uint32_t pos){
	DatosPersona persona;

	char _dni[9];
	char num[10];
	
	DiskManager *diskM = DiskManager::getInstance();
	uint32_t posDisco = pos;
	// dni
	diskM->readDisk(posDisco,_dni,sizeof(_dni));
	posDisco += sizeof(_dni) + 1;
	std::cout << _dni << " | ";
	// nombres
	diskM->readDisk(posDisco,persona.nombres,sizeof(persona.nombres));
	posDisco += sizeof(persona.nombres) + 1;
	std::cout << persona.nombres << " | ";
	// apellidos
	diskM->readDisk(posDisco,persona.apellidos,sizeof(persona.apellidos));
	posDisco += sizeof(persona.apellidos) + 1;
	std::cout << persona.apellidos << " | ";
	// direccion
	diskM->readDisk(posDisco,persona.direccion,sizeof(persona.direccion));
	posDisco += sizeof(persona.direccion) + 1;
	std::cout << persona.direccion << " | ";
	// nacimiento
	diskM->readDisk(posDisco,persona.nacimiento,sizeof(persona.nacimiento));
	posDisco += sizeof(persona.nacimiento) + 1;
	std::cout << persona.nacimiento << " | ";
	// nacionalidad
	diskM->readDisk(posDisco,persona.nacionalidad,sizeof(persona.nacionalidad));
	posDisco += sizeof(persona.nacionalidad) + 1;
	std::cout << persona.nacionalidad << " | ";
	// lugarnacimiento
	diskM->readDisk(posDisco,persona.lugarnacimiento,sizeof(persona.lugarnacimiento));
	posDisco += sizeof(persona.lugarnacimiento) + 1;
	std::cout << persona.lugarnacimiento << " | ";
	// telefono
	diskM->readDisk(posDisco,num,sizeof(num));
	posDisco += sizeof(num) + 1;
	std::cout << num << " | ";
	// correo
	diskM->readDisk(posDisco,persona.correo,sizeof(persona.correo));
	posDisco += sizeof(persona.correo) + 1;
	std::cout << persona.correo << " | ";
	// correo
	diskM->readDisk(posDisco,persona.estadoCivil,sizeof(persona.estadoCivil));
	posDisco += sizeof(persona.estadoCivil) + 1;
	std::cout << persona.estadoCivil << "\n";
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

int main()
{
    CuckooHashing* cuckoo = new CuckooHashing(INITIAL_TABLE_SIZE);
    std::cout<<"[MAIN] cuckoo reference "<< cuckoo << std::endl;
    // comprobamos si el .bin existe
    std::ifstream file("cuckohash.bin", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[MAIN] cuckohash.bin no existe, buscando personas.txt\n";
        LoadCuckoo load;
        load.firstWrite(cuckoo,"personas_100.txt");
        std::cout<<"[MAIN] Exito, generando el archivo"<<std::endl;
    }  else {
        std::cout<<"[MAIN] Cargar cuckoo en nuevo vector..."<<std::endl;
        if(cuckoo->readFile()){
        	std::cout<<"[MAIN] Vector online"<<std::endl;
        	std::cout<<"[TEST] Imprimiendo los primeros 100 valores del vector"<<std::endl;
  			cuckoo->printVector(100);
        } else {
        	std::cout<<"[ERROR] No se leyo!"<<std::endl;
        }
    }
    
    
    std::cout << "[TESTING] probando con DNI # 21047390" << std::endl;
   	DniPos dniPos = cuckoo->searchDNI(21047390);
   	if(dniPos.dni == 0) {std::cout << "DNI no existente" <<std::endl;} else {
   		ReadPerson(dniPos.dni);
   	}
    

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
