// Queremos que este manager solo exista una vez
// Usaremos singleton
#include <mutex>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include <memory>
class DiskManager{
    private:
        static std::unique_ptr<DiskManager> m_pinstance;
        static std::mutex m_mutex;
        const char m_dir[10] = "/dev/sda5";
    protected:
        DiskManager() = default;
    public:
        DiskManager(DiskManager &other) = delete;
        void operator=(const DiskManager &) = delete;
        static DiskManager* getInstance();
        
        void writeDisk(uint32_t byte, const char (&dato)[], short size, uint32_t dni = 0){
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

                if(dni != 0){
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
                } else {

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

        bool readDisk(uint32_t byte, char(&dato_llenar)[], short size){
            try {
                int fd = open(m_dir, O_RDONLY);
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
                return false;
            }
            catch ( const std::system_error& e ) {
                std::cerr << e.what() << "NO se leyo correctamente, abortando..." << std::endl;
                std::cin.get();
                exit(-1);
            }
        }
};

