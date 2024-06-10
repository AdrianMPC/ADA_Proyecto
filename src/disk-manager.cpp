#include <fcntl.h>
#include "../models/disk-manager.h"

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
