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

void DiskManager::writeDisk(uint32_t byte, char (&dato)[], short size){
    try {
        int fd = open(m_dir, O_WRONLY);
        if(fd == -1){
            throw std::invalid_argument(m_dir); // Salir del programa con código de error
        }
        if (lseek(fd, byte + size, SEEK_SET) == -1) {
            close(fd);
            std::cerr << "Failed writing for " << dato << "with size: " << size << "in byte: " << byte <<"\n";
            throw std::system_error();
        }
        ssize_t bytes_written = write(fd, dato, size);
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
 
    // Asegurarse de que se haya abierto correctamente antes de continuar
    if (lseek(fd, 1024 * 512, SEEK_SET) == -1) {
        printf("Error al buscar en el archivo: %s\n", strerror(errno));
        close(fd); // cerrar el descriptor de archivo antes de salir
        return 1;
    }

    char buffer[] = "Hello, disca";
    ssize_t bytes_written = write(fd, buffer, sizeof(buffer));
    if (bytes_written == -1) {
        printf("Error al escribir en el archivo: %s\n", strerror(errno));
        close(fd); // cerrar el descriptor de archivo antes de salir
        return 1;
    } else if (bytes_written != sizeof(buffer)) {
        printf("No se escribió toda la información en el archivo.\n");
        close(fd); // cerrar el descriptor de archivo antes de salir
        return 1;
    }

    if (close(fd) == -1) {
        printf("Error al cerrar el archivo: %s\n", strerror(errno));
        return 1;
    }

    printf("Se ha escrito: %s \n", buffer);
    return 0;
}