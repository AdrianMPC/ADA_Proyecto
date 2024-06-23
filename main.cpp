#include <iostream>
#include <fstream>
#include <sstream>
#include <signal.h>
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
#define INITIAL_TABLE_SIZE 70000000
CuckooHashing* cuckoo = new CuckooHashing(INITIAL_TABLE_SIZE);

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

void signalHandler(int signum) {
   	if (cuckoo->writeFile()){
    	std::cout << "[PersonaManager] cuckoohash.bin se genero correctamente\n";
    } else {
    	std::cout << "[PersonaManager] cuckoohash.bin no se genero correctamente\n";
    }
    exit(signum);
}


int main()
{
	//TODO Cambiar esto para que no sea una aberracion en el main (talvez una clase)
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    PersonaManager personaManager(cuckoo);
    // comprobamos si el .bin existe
    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream file("cuckohash.bin", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[MAIN-LOG] cuckohash.bin no existe, buscando personas.txt\n";
        LoadCuckoo load;
        load.firstWrite(cuckoo,"personas_100.txt");
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
    auto end = std::chrono::high_resolution_clock::now();

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    auto duration_sec = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    // Extract milliseconds and seconds
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration_ms).count() % 1000;
    auto sec = duration_sec.count();

    std::cout << "Time taken: " << sec << " seconds and " << ms << " milliseconds\n";
  	
  	int choice;
    do {
        printMenu();
        std::cin >> choice;
        std::cin.ignore(); // Ignore newline character left in the buffer

        switch (choice) {
            case 1: {
            	auto start = std::chrono::high_resolution_clock::now();
               	uint32_t dni = askDNI();
    			DniPos dniPos = cuckoo->searchDNI(dni);
			
    			std::cout << "[MAIN-LOG] Buscando DNI: " << dniPos.dni << " y POS: " << dniPos.pos << std::endl;
    			
    			if (dniPos.dni == 0) {
        			std::cout << "[MAIN-LOG] DNI no encontrado" << std::endl;
    			} else {
        			personaManager.readPerson(dniPos.pos);  
    			}
    			
				auto end = std::chrono::high_resolution_clock::now();
    			auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    			auto duration_sec = std::chrono::duration_cast<std::chrono::seconds>(end - start);
			
    			// Extract milliseconds and seconds
    			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration_ms).count() % 1000;
    			auto sec = duration_sec.count();
			
    			std::cout << "Time taken: " << sec << " seconds and " << ms << " milliseconds\n";
                break;
            }
            case 2: {
            	auto start = std::chrono::high_resolution_clock::now();
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
                auto end = std::chrono::high_resolution_clock::now();
    			auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    			auto duration_sec = std::chrono::duration_cast<std::chrono::seconds>(end - start);
			
    			// Extract milliseconds and seconds
    			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration_ms).count() % 1000;
    			auto sec = duration_sec.count();
			
    			std::cout << "Time taken: " << sec << " seconds and " << ms << " milliseconds\n";
                break;
            }
            case 3: {
            	auto start = std::chrono::high_resolution_clock::now();
                uint32_t dni;
                std::cout << "Enter DNI to delete: ";
                std::cin >> dni;
                if (personaManager.deletePerson(dni)) {
                    std::cout << "Person deleted successfully.\n";
                } else {
                    std::cout << "Error deleting person.\n";
                }
                auto end = std::chrono::high_resolution_clock::now();
    			auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    			auto duration_sec = std::chrono::duration_cast<std::chrono::seconds>(end - start);
			
    			// Extract milliseconds and seconds
    			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration_ms).count() % 1000;
    			auto sec = duration_sec.count();
			
    			std::cout << "Time taken: " << sec << " seconds and " << ms << " milliseconds\n";
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
            DatosPersona datosPersona;
            DniPos dniPos = cuckoo->searchDNI(dni);
            if(dniPos.dni == 0){throw std::runtime_error("");}
            datosPersona = personaManager.readPerson(dniPos.pos);  
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
             }
                );
        }
        catch (const std::runtime_error &err)
        {
            return crow::response(404, "DNI Not Found");
        } });

    // POST PERSON ROUTE
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
            if(!personaManager.writePerson(datosPersona)){
            	throw std::exception();
            }
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
            return crow::response(400, "Error al escribir los datos");
        } });


    // DELETE PERSON ROUTE
    // THIS ROUTE WILL RECEIVE A JSON WITH A DNI AND WILL DELETE THE PERSON IN THE HARD DRIVE
    CROW_ROUTE(app, "/api/person").methods(crow::HTTPMethod::DELETE)([&](const crow::request &req)
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
            if(personaManager.deletePerson(dni)){
            	return crow::response(200, "Person deleted successfully");
            } 
            throw std::exception();
        }
        catch (const std::exception &err)
        {
            return crow::response(404, "DNI Not Found");
        } });



    app.port(3000)
        .multithreaded()
        .run();

    return 0;
}


