#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include "../models/disk-manager.h"
#define array_size(arr) (sizeof(arr) / sizeof(*(arr)))
std::unique_ptr<DiskManager> DiskManager::m_pinstance{nullptr};
std::mutex DiskManager::m_mutex;

//  La primer vez que llamamos a getInstance bloquearemos la ubicacion de la data
//  luego verificamo si existe la variable o no
DiskManager* DiskManager::getInstance() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_pinstance) {
        m_pinstance = std::unique_ptr<DiskManager>(new DiskManager());
    }
    return m_pinstance.get();
}

