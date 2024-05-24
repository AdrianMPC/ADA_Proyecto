#pragma once

#include <vector>
#include <cstdint>
#include "personamodelo.h"

class BTreeNode{
    private:
        short m_minDegree;
        std::vector<uint32_t> m,_keys;
        std::vector<BTreeNode*> m_children;
        uint32_t m_numKeys;
        bool m_isLeaf;
    public:
        BTreeNode(short _minDegree, bool _isLeaf);
        void traverse();
        void insertNonFull(uint32_t dni);
        void splitChild(uint32_t index, BTreeNode* childNode);
        friend class BTree;
};

class BTree{
    private:
        BTreeNode* root;
        short m_minDegree;
    public: 
        BTree(short _minDegree);
        BTreeNode* getRoot() const;
        void traverse();
        void insertPerson(uint32_t dni);
        DatosPersona* searchDNI(uint32_t dni); 
        void deleteDNI(uint32_t dni);
};