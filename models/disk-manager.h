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
        template<typename T>
            void writeDisk(uint32_t byte, const T& dato, short size);
        template<typename T>
            bool readDisk(uint32_t byte, T& dato_llenar, short size = 40);
};