#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include "../models/disk-manager.h"
#define array_size(arr) (sizeof(arr) / sizeof(*(arr)))
DiskManager* DiskManager::m_pinstance{nullptr};
std::mutex DiskManager::m_mutex;

//  La primer vez que llamamos a getInstance bloquearemos la ubicacion de la data
//  luego verificamo si existe la variable o no
DiskManager *DiskManager::getInstance(){
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_pinstance == nullptr){
        m_pinstance = new DiskManager();
    }
    return m_pinstance;
}
template<>
void DiskManager::writeDisk(uint32_t byte, const char (&dato)[], short size){
    try {
        int fd = open(m_dir, O_WRONLY);
        if(fd == -1){
            throw std::invalid_argument(m_dir); // Salir del programa con código de error
        }
        if (lseek(fd, byte, SEEK_SET) == -1) {
            close(fd);
            std::cerr << "Failed SET for " << dato << "with size: " << size << "in byte: " << byte <<"\n";
            throw std::system_error();
        }
        ssize_t bytes_written = write(fd, dato, size);
        if (bytes_written == -1) {
            close(fd);
            std::cerr << "Failed writing for " << dato << "with size: " << size << "in byte: " << byte <<"\n";
            throw std::system_error();
        }
        if (bytes_written != size) {
            std::cerr << "Overflow in writing for " << dato << "with size: " << size << "in byte: " << byte <<"\n";
            throw std::system_error();
        }
        if (close(fd) == -1) {
            std::cerr << "No se cerro el disco" << std::endl;
        }
    } 
    catch ( const std::invalid_argument& e ) {
        std::cerr << e.what() << " | No se pudo abrir la particion \n Cerrando programa \n resione cualquier tecla"<<std::endl;
        std::cin.get();
        exit(-1);
    }
    catch ( const std::system_error& e ) {
        std::cerr << e.what() << "NO se escribio correctamente, abortando..." << std::endl;
        std::cin.get();
        exit(-1);
    }
}

template<>
void DiskManager::writeDisk<uint32_t>(uint32_t byte, const uint32_t& dato, short size) {
    try {
        int fd = open(m_dir, O_WRONLY);
        if (fd == -1) {
            throw std::invalid_argument(m_dir);
        }
        if (lseek(fd, byte, SEEK_SET) == -1) {
            close(fd);
            std::cerr << "Failed SET for uint32_t in byte: " << byte << "\n";
            throw std::system_error();
        }
        ssize_t bytes_written = write(fd, &dato, sizeof(dato));
        if (bytes_written == -1) {
            close(fd);
            std::cerr << "Failed writing uint32_t in byte: " << byte << "\n";
            throw std::system_error();
        }
        if (bytes_written != sizeof(dato)) {
            std::cerr << "Overflow in writing uint32_t in byte: " << byte << "\n";
            throw std::system_error();
        }
        if (close(fd) == -1) {
            std::cerr << "No se cerro el disco" << std::endl;
        }
    } 
    catch ( const std::invalid_argument& e ) {
        std::cerr << e.what() << " | No se pudo abrir la particion \n Cerrando programa \n resione cualquier tecla"<<std::endl;
        std::cin.get();
        exit(-1);
    }
    catch ( const std::system_error& e ) {
        std::cerr << e.what() << "NO se escribio correctamente, abortando..." << std::endl;
        std::cin.get();
        exit(-1);
    }
}

template<>
bool DiskManager::readDisk(uint32_t byte, char(&dato_llenar)[], short size){
    try {
        int fd = open(m_dir, O_WRONLY);
        if(fd == -1){
            throw std::invalid_argument(m_dir); // Salir del programa con código de error
        }
        char buffer_read[size];

        if (lseek(fd, byte, SEEK_SET) == -1) {
            close(fd);
            std::cerr << "[READ] Failed SET for " << byte << "with size: " << size << "\n";
            throw std::system_error();
        }
        short bytes_read = read(fd, buffer_read, sizeof(buffer_read) - 1);
        if (bytes_read == -1) {
            close(fd);
            std::cerr << "[READ] Failed reading for " << dato_llenar << "with size: " << size << "in byte: " << byte <<"\n";
            throw std::system_error();
        } else {
          // LEYENDO
          for(short pointer = 0; pointer < size; pointer++){
            dato_llenar[pointer] = buffer_read[pointer];
          }
          return true;
        }
        if (close(fd) == -1) {
            std::cerr << "No se cerro el disco" << std::endl;
            return false;
        }
    } 
    catch ( const std::invalid_argument& e ) {
        std::cerr << e.what() << " | No se pudo abrir la particion \n Cerrando programa \n presione cualquier tecla"<<std::endl;
        std::cin.get();
    }
    catch ( const std::system_error& e ) {
        std::cerr << e.what() << "NO se leyo correctamente, abortando..." << std::endl;
        std::cin.get();
        exit(-1);
    }
}

template<>
bool DiskManager::readDisk<uint32_t>(uint32_t byte, uint32_t& dato_llenar, short size) {
    try {
        int fd = open(m_dir, O_RDONLY);
        if (fd == -1) {
            throw std::invalid_argument(m_dir);
        }
        if (lseek(fd, byte, SEEK_SET) == -1) {
            close(fd);
            std::cerr << "[READ] Failed SET for uint32_t in byte: " << byte << "\n";
            throw std::system_error();
        }
        ssize_t bytes_read = read(fd, &dato_llenar, sizeof(dato_llenar));
        if (bytes_read == -1) {
            close(fd);
            std::cerr << "[READ] Failed reading uint32_t in byte: " << byte << "\n";
            throw std::system_error();
        }
        if (bytes_read != sizeof(dato_llenar)) {
            std::cerr << "[READ] Incomplete read for uint32_t in byte: " << byte << "\n";
            throw std::system_error();
        }
        if (close(fd) == -1) {
            std::cerr << "No se cerro el disco" << std::endl;
        }
        return true;
    }
    catch ( const std::invalid_argument& e ) {
        std::cerr << e.what() << " | No se pudo abrir la particion \n Cerrando programa \n presione cualquier tecla"<<std::endl;
        std::cin.get();
    }
    catch ( const std::system_error& e ) {
        std::cerr << e.what() << "NO se leyo correctamente, abortando..." << std::endl;
        std::cin.get();
        exit(-1);
    }
}