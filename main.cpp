#include <iostream>
#include <fstream>
#include <sstream>
#include "models/btree.h"



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

int main() {
    return 0;
}
