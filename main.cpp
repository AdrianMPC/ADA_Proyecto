#include <iostream>
#include <fstream>
#include <sstream>
#include "models/btree.h"
#include "models/disk-manager.h"



uint32_t parseLine(const std::string& line){
    std::stringstream ss(line);
    std::string token;
    std::getline(ss, token, ';');
    return std::stoul(token);
}

void loadDataAndTest(BTree& tree, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }
    std::string line;

    while (std::getline(file, line)) {
        uint32_t person = parseLine(line);
        tree.insertPerson(person);
    }

    file.close();
}

void BTreeTesting(){
    BTree tree(3); 
    std::string file = "../personas.txt";
    loadDataAndTest(tree, file);
    std::cout << "Datos cargados...\n";
    std::cout << "Presiona cualquier tecla para continuar...\n";
    std::cin.get();

    uint32_t dniToSearch = 26854037; 
    std::cout << "Buscando DNI: " << dniToSearch << "\n";

    if(tree.searchDNI(dniToSearch)){
        std::cout<<"SI se encontró el DNI #"<<dniToSearch<<"\n";
    } else {
        std::cout<<"NO se encontró el DNI #"<<dniToSearch<<"\n";
    }

    tree.deleteDNI(dniToSearch);

    if(tree.searchDNI(dniToSearch)){
        std::cout<<"SI se encontró el DNI #"<<dniToSearch<<"\n";
    } else {
        std::cout<<"NO se encontró el DNI #"<<dniToSearch<<"\n";
    }
}

// LA FORMULA PARA DETERMINAR QUE POSICION USAR SIGUIENTE ES:
// (TAMAÑO DE DATOS (QUE YA INCLUYE EL \0) + 1)
// 
void WriteDiskTest(){
    DiskManager* diskM = DiskManager::getInstance();
    int dataso = 70799417;
    char dataso_leer[9];
    char test[10] = "hola12345"; // char de 9 + 1 el null
    char test_leer[10];
    diskM->borrarDatosNull(1024,sizeof(dataso_leer));
    diskM->writeDiskInt(1024,dataso);
    diskM->borrarDatosNull(1034,sizeof(test));
    diskM->writeDisk(1034,test);
    diskM->readDisk(1024,dataso_leer,sizeof(dataso_leer));
    diskM->readDisk(1034,test_leer,sizeof(test_leer));
    std::cout<<dataso_leer<<"\n";
    std::cout<<test_leer<<"\n";   
}

int main() {
    WriteDiskTest();
    //int dato = 42;
    //char buffer[2]; // Suficientemente grande para contener el entero como una cadena
    
   // sprintf(buffer, "%d", dato); // Convertir el entero a una cadena
    
    //std::cout<<buffer;
    return 0;
}
