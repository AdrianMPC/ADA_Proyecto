// Queremos que este manager solo exista una vez
// Usaremos singleton
#pragma once
#include <mutex>
class DiskManager{
    private:
        static DiskManager* m_pinstance;
        static std::mutex m_mutex;
        const char m_dir[10] = "/dev/sda5";
    protected:
        DiskManager() = default;
        ~DiskManager() = default;
    public:
        DiskManager(DiskManager &other) = delete;
        void operator=(const DiskManager &) = delete;
        static DiskManager* getInstance();
        void writeDisk(uint32_t byte, char(&dato)[],short size);
        bool readDisk(uint32_t byte, char(&dato_llenar)[]);
};