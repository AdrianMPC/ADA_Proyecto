#pragma once

#include <vector>
#include <cstdint>
#include "personamodelo.h"

class BTreeNode{
    private:
        short m_minDegree;
        std::vector<uint32_t> m_keys;
        std::vector<BTreeNode*> m_children;
        uint32_t m_numKeys;
        bool m_isLeaf;
        bool search(uint32_t dni);
        void remove(uint32_t dni);
        void removeFromLeaf(int32_t index);
        void removeFromNonLeaf(int32_t index);
        uint32_t getPredecessor(int32_t index);
        uint32_t getSuccessor(int32_t index);
        void fill(int32_t index);
        void borrowFromPrev(int32_t index);
        void borrowFromNext(int32_t index);
        void merge(int32_t index);
    public:
        BTreeNode(short _minDegree, bool _isLeaf);
        void traverse();
        void insertNonFull(uint32_t person);
        void splitChild(int32_t index, BTreeNode* childNode);
        friend class BTree;
};

class BTree{
    private:
        BTreeNode* m_root;
        short m_minDegree;
    public: 
        BTree(short _minDegree);
        BTreeNode* getRoot() const;
        void traverse();
        void insertPerson(uint32_t dni);
        bool searchDNI(uint32_t dni); 
        void deleteDNI(uint32_t dni);
};