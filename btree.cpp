#include "btree.h"

BTreeNode::BTreeNode(short _minDegree, bool _isLeaf){ 
}

void BTreeNode::traverse() {
}

void BTreeNode::insertNonFull(uint32_t dni) {
}

void BTreeNode::splitChild(uint32_t index, BTreeNode* childNode) {
}

// implementacion btree

BTree::BTree(short _minDegree) {}

BTreeNode* BTree::getRoot() const {
    return root;
}

void BTree::traverse() {
    if (root != nullptr) {
        root->traverse();
    }
}

void BTree::insertPerson(uint32_t dni) {
}

DatosPersona* BTree::searchDNI(uint32_t dni) {
}

void BTree::deleteDNI(uint32_t dni) {
}