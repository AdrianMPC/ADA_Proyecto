#pragma once

#include <sys/stat.h>
#include <mutex>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <cstring>


class DiskManager {
private:
    static std::unique_ptr<DiskManager> m_pinstance;
    static std::mutex m_mutex;
    const char* m_dir = "/dev/sda4";

protected:
    DiskManager() = default;

public:
    DiskManager(DiskManager &other) = delete;
    void operator=(const DiskManager &) = delete;

    static DiskManager* getInstance();
    
    bool checkPartition(const char* partitionPath) {
    	struct stat stat_buf;
    	if (stat(partitionPath, &stat_buf) == -1) {
        	std::cerr << "Error: La partición " << partitionPath << " no existe o no está accesible.\n";
        	return false;
    	}
    	if (!S_ISBLK(stat_buf.st_mode)) {
        	std::cerr << "Error: " << partitionPath << " no es un dispositivo de bloque.\n";
        	return false;
    	}

    	if (access(partitionPath, W_OK) == -1) {
        	std::cerr << "Error: No tienes permisos de escritura en la partición " << partitionPath << ".\n";
        	return false;
    	}
    	return true;
	}
    
    off_t getPartitionSize(const char* partitionPath) {
    	struct stat stat_buf;
    	if (stat(partitionPath, &stat_buf) == -1) {
        	std::cerr << "Error: No se pudo obtener el tamaño de la partición.\n";
        	return -1;
    	}
    	return stat_buf.st_size;
	}

    void borrarDatosNull(uint32_t byte, short size) {
        try {
            int fd = open(m_dir, O_WRONLY);
            if (fd == -1) {
                throw std::invalid_argument(m_dir);
            }
            if (lseek(fd, byte, SEEK_SET) == -1) {
                close(fd);
                std::cerr << " - [CLEAN] Failed SET pre-cleaning with size: " << size << " in byte: " << byte << "\n";
                throw std::runtime_error("Error de posicionamiento de archivo");
            }

            char buffer[size];
            memset(buffer, 0, sizeof(buffer));

            ssize_t bytes_written = write(fd, buffer, sizeof(buffer));
            if (bytes_written == -1) {
                close(fd);
                std::cerr << " - [CLEAN] Failed writing pre-cleaning with size: " << size << " in byte: " << byte << "\n";
                throw std::runtime_error("Error de escritura");
            }
            if (bytes_written != sizeof(buffer)) {
                std::cerr << " - [CLEAN] Overflow in writing pre-cleaning with size: " << size << " in byte: " << byte << "\n";
                throw std::runtime_error("Escritura incompleta");
            }
            if (close(fd) == -1) {
                std::cerr << " - [CLEAN] No se cerro el disco" << std::endl;
            }
        }
        catch (const std::invalid_argument& e) {
            std::cerr << e.what() << " | No se pudo abrir la particion \n Cerrando programa \n presione cualquier tecla" << std::endl;
            std::cin.get();
            exit(-1);
        }
        catch (const std::runtime_error& e) {
            std::cerr << e.what() << " | No se escribio correctamente, abortando!" << std::endl;
            std::cin.get();
            exit(-1);
        }
    }

    void writeDisk(uint32_t byte, const char* dato) {
        try {
        		// Verificar que la partición es accesible y writable
    		if (!checkPartition(m_dir)) {
        		throw std::runtime_error("Partición no accesible o no writable");
    		}
    		
    		// Obtener el tamaño de la partición
        	//off_t partitionSize = getPartitionSize(m_dir);
        	//if (partitionSize == -1) {
           // 	throw std::runtime_error("Error al obtener el tamaño de la partición");
        	//}
        	//if (byte >= partitionSize) {
           // 	std::cerr << "Error: El byte " << byte << " está fuera de los límites de la partición -> "<<partitionSize <<"\n";
           // 	throw std::runtime_error("Intento de posicionarse fuera de los límites de la partición");
        //	}
		
    		// Abrir la partición
    		int fd = open(m_dir, O_WRONLY);
    		if (fd == -1) {
        		throw std::invalid_argument("No se pudo abrir la partición");
    		}

            if (lseek(fd, byte, SEEK_SET) == -1) {
                close(fd);
                std::cerr << " - [WRITE-CHAR] Failed SET for " << dato << " with size: " << strlen(dato) << " in byte: " << byte << "\n";
                throw std::runtime_error("Error de posicionamiento de archivo");
            }
            ssize_t bytes_written = write(fd, dato, strlen(dato));

            if (bytes_written == -1) {
                close(fd);
                std::cerr << " - [WRITE-CHAR] Failed writing for " << dato << " with size: " << strlen(dato) << " in byte: " << byte << "\n";
                throw std::runtime_error("Error de escritura");
            }
            if (bytes_written != (ssize_t)strlen(dato)) {
                std::cerr << " - [WRITE-CHAR] Overflow in writing for " << dato << " with size: " << strlen(dato) << " in byte: " << byte << "\n";
                throw std::runtime_error("Escritura incompleta");
            }
            if (close(fd) == -1) {
                std::cerr << " - [WRITE-CHAR] No se cerro el disco" << std::endl;
                throw std::runtime_error("Archivo abierto");
            }
        }
        catch (const std::invalid_argument& e) {
            std::cerr << e.what() << " | No se pudo abrir la particion \n Cerrando programa \n presione cualquier tecla" << std::endl;
            std::cin.get();
            exit(-1);
        }
        catch (const std::runtime_error& e) {
            std::cerr << e.what() << " | NO se escribio correctamente, abortando..." << std::endl;
            std::cin.get();
            exit(-1);
        }
    }

    void writeDiskInt(uint32_t byte, const uint32_t dato) {
        try {
            int fd = open(m_dir, O_WRONLY);
            if (fd == -1) {
                throw std::invalid_argument(m_dir);
            }
            if (lseek(fd, byte, SEEK_SET) == -1) {
                close(fd);
                std::cerr << " - [INT-WRITE] Failed SET for " << dato << " in byte: " << byte << "\n";
                throw std::runtime_error("Error de posicionamiento de archivo");
            }

            short numDigits = snprintf(NULL, 0, "%d", dato);
            char buffer[numDigits + 1];
            snprintf(buffer, numDigits + 1, "%d", dato);
            ssize_t bytes_written = write(fd, buffer, strlen(buffer));

            if (bytes_written == -1) {
                close(fd);
                std::cerr << " - [INT-WRITE] Failed writing for " << dato << " with size: " << numDigits << " in byte: " << byte << "\n";
                throw std::runtime_error("Error de escritura");
            }
            if (bytes_written != (ssize_t)strlen(buffer)) {
                std::cerr << "- [INT-WRITE] Overflow in writing for " << dato << " with size: " << numDigits << " in byte: " << byte << "\n";
                throw std::runtime_error("Escritura incompleta");
            }
            if (close(fd) == -1) {
                std::cerr << "- [INT-WRITE] No se cerro el disco" << std::endl;
            }
        }
        catch (const std::invalid_argument& e) {
            std::cerr << e.what() << " | No se pudo abrir la particion \n Cerrando programa \n presione cualquier tecla" << std::endl;
            std::cin.get();
            exit(-1);
        }
        catch (const std::runtime_error& e) {
            std::cerr << e.what() << " | No se escribio correctamente, abortando..." << std::endl;
            std::cin.get();
            exit(-1);
        }
    }

    bool readDisk(uint32_t byte, char* dato_llenar, short size) {
        try {
            int fd = open(m_dir, O_RDONLY);
            if (fd == -1) {
                throw std::invalid_argument(m_dir);
            }
            char buffer_read[size];

            if (lseek(fd, byte, SEEK_SET) == -1) {
                close(fd);
                std::cerr << "- [READ] Failed SET for " << byte << " with size: " << size << "\n";
                throw std::runtime_error("Error de posicionamiento de archivo");
            }
            ssize_t bytes_read = read(fd, buffer_read, size - 1);
            if (bytes_read == -1) {
                close(fd);
                std::cerr << " - [READ] Failed reading for " << dato_llenar << " with size: " << size << " in byte: " << byte << "\n";
                throw std::runtime_error("Error de lectura");
            } else {
                for (short pointer = 0; pointer < size - 1; pointer++) {
                    dato_llenar[pointer] = buffer_read[pointer];
                    //std::cout<<"Dato a llenar -> "<< dato_llenar[pointer]  << " BUFFER -> " << buffer_read[pointer] << std::endl;
                }
                //std::cout<<"Agregando null en : "<<size - 1<<std::endl;
                dato_llenar[size - 1] = '\0';
                return true;
            }
            if (close(fd) == -1) {
                std::cerr << " - [READ] No se cerro el disco" << std::endl;
                throw std::runtime_error("Archivo no cerrado!");
            }
        }
        catch (const std::invalid_argument& e) {
            std::cerr << e.what() << " | No se pudo abrir la particion \n Cerrando programa, presione cualquier tecla..." << std::endl;
            std::cin.get();
            return false;
        }
        catch (const std::runtime_error& e) {
            std::cerr << e.what() << " | No se leyo correctamente, abortando..." << std::endl;
            std::cin.get();
            return false;
        }
    }
};
