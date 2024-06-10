#include "../models/btree.h"
#include "../models/personamodelo.h"
#include <iostream>

BTreeNode::BTreeNode(short _minDegree, bool _isLeaf){ 
    m_minDegree = _minDegree;
    m_isLeaf = _isLeaf;
    m_numKeys = 0;
    m_keys.resize(2*_minDegree-1);
    m_children.resize(2*_minDegree);
}

void BTreeNode::traverse() {
    uint32_t i;
    for (i = 0; i < m_numKeys; i++) {
        if (!m_isLeaf)
            m_children[i]->traverse();
        std::cout << " " << m_keys[i];
    }
    if (!m_isLeaf)
        m_children[i]->traverse();
}

bool BTreeNode::search(uint32_t dni) {
    uint32_t i = 0;
    while (i < m_numKeys && dni > m_keys[i])
        i++;
    if (i < m_numKeys && m_keys[i] == dni)
        return true;
    if (m_isLeaf)
        return false;
    return m_children[i]->search(dni);
}

void BTreeNode::insertNonFull(uint32_t key) {
    int32_t index = m_numKeys - 1;
    if (m_isLeaf) {
        while (index >= 0 && m_keys[index] > key) {
            m_keys[index + 1] = m_keys[index];
            index--;
        }
        m_keys[index + 1] = key;
        m_numKeys++;
    } else {
        while (index >= 0 && m_keys[index] > key)
            index--;
        if (m_children[index + 1]->m_numKeys == 2 * m_minDegree - 1) {
            splitChild(index + 1, m_children[index + 1]);
            if (m_keys[index + 1] < key)
                index++;
        }
        m_children[index + 1]->insertNonFull(key);
    }
}


void BTreeNode::splitChild(int32_t index, BTreeNode* childNode) {
    BTreeNode* newChild = new BTreeNode(childNode->m_minDegree, childNode->m_isLeaf);
    newChild->m_numKeys = m_minDegree - 1;

    for (short j = 0; j < m_minDegree - 1; j++) {
        newChild->m_keys[j] = childNode->m_keys[j + m_minDegree];
    }

    if (!childNode->m_isLeaf) {
        for (short j = 0; j < m_minDegree; j++)
            newChild->m_children[j] = childNode->m_children[j + m_minDegree];
    }
    childNode->m_numKeys = m_minDegree - 1;

    for (short j = m_numKeys; j >= index + 1; j--) {
        m_children[j + 1] = m_children[j];
    }
    m_children[index + 1] = newChild;

    for (short j = m_numKeys - 1; j >= index; j--) {
        m_keys[j + 1] = m_keys[j];
    }
    m_keys[index] = childNode->m_keys[m_minDegree - 1];

    m_numKeys++;
}

void BTreeNode::remove(uint32_t key) {
    int32_t index = 0;
    while (index < m_numKeys && m_keys[index] < key)
        index++;

    if (index < m_numKeys && m_keys[index] == key) {
        if (m_isLeaf)
            removeFromLeaf(index);
        else
            removeFromNonLeaf(index);
    } else {
        if (m_isLeaf) {
            std::cout << "La llave " << key << " no existe en el árbol\n";
            return;
        }

        bool flag = (index == m_numKeys);
        if (m_children[index]->m_numKeys < m_minDegree)
            fill(index);

        if (flag && index > m_numKeys)
            m_children[index - 1]->remove(key);
        else
            m_children[index]->remove(key);
    }
}

void BTreeNode::removeFromLeaf(int32_t index) {
    for (int32_t i = index + 1; i < m_numKeys; ++i)
        m_keys[i - 1] = m_keys[i];
    m_numKeys--;
}

void BTreeNode::removeFromNonLeaf(int32_t index) {
    uint32_t key = m_keys[index];

    if (m_children[index]->m_numKeys >= m_minDegree) {
        uint32_t pred = getPredecessor(index);
        m_keys[index] = pred;
        m_children[index]->remove(pred);
    } else if (m_children[index + 1]->m_numKeys >= m_minDegree) {
        uint32_t succ = getSuccessor(index);
        m_keys[index] = succ;
        m_children[index + 1]->remove(succ);
    } else {
        merge(index);
        m_children[index]->remove(key);
    }
}

uint32_t BTreeNode::getPredecessor(int32_t index) {
    BTreeNode* cur = m_children[index];
    while (!cur->m_isLeaf)
        cur = cur->m_children[cur->m_numKeys];
    return cur->m_keys[cur->m_numKeys - 1];
}

uint32_t BTreeNode::getSuccessor(int32_t index) {
    BTreeNode* cur = m_children[index + 1];
    while (!cur->m_isLeaf)
        cur = cur->m_children[0];
    return cur->m_keys[0];
}

void BTreeNode::fill(int32_t index) {
    if (index != 0 && m_children[index - 1]->m_numKeys >= m_minDegree)
        borrowFromPrev(index);
    else if (index != m_numKeys && m_children[index + 1]->m_numKeys >= m_minDegree)
        borrowFromNext(index);
    else {
        if (index != m_numKeys)
            merge(index);
        else
            merge(index - 1);
    }
}

void BTreeNode::borrowFromPrev(int32_t index) {
    BTreeNode* child = m_children[index];
    BTreeNode* sibling = m_children[index - 1];

    for (int32_t i = child->m_numKeys - 1; i >= 0; --i)
        child->m_keys[i + 1] = child->m_keys[i];

    if (!child->m_isLeaf) {
        for (int32_t i = child->m_numKeys; i >= 0; --i)
            child->m_children[i + 1] = child->m_children[i];
    }

    child->m_keys[0] = m_keys[index - 1];
    if (!child->m_isLeaf)
        child->m_children[0] = sibling->m_children[sibling->m_numKeys];

    m_keys[index - 1] = sibling->m_keys[sibling->m_numKeys - 1];
    child->m_numKeys += 1;
    sibling->m_numKeys -= 1;
}


void BTreeNode::borrowFromNext(int32_t index) {
    BTreeNode* child = m_children[index];
    BTreeNode* sibling = m_children[index + 1];

    child->m_keys[child->m_numKeys] = m_keys[index];

    if (!child->m_isLeaf)
        child->m_children[child->m_numKeys + 1] = sibling->m_children[0];

    m_keys[index] = sibling->m_keys[0];

    for (int32_t i = 1; i < sibling->m_numKeys; ++i)
        sibling->m_keys[i - 1] = sibling->m_keys[i];

    if (!sibling->m_isLeaf) {
        for (int32_t i = 1; i <= sibling->m_numKeys; ++i)
            sibling->m_children[i - 1] = sibling->m_children[i];
    }

    child->m_numKeys += 1;
    sibling->m_numKeys -= 1;
}

void BTreeNode::merge(int32_t index) {
    BTreeNode* child = m_children[index];
    BTreeNode* sibling = m_children[index + 1];

    child->m_keys[m_minDegree - 1] = m_keys[index];

    for (int32_t i = 0; i < sibling->m_numKeys; ++i)
        child->m_keys[i + m_minDegree] = sibling->m_keys[i];

    if (!child->m_isLeaf) {
        for (int32_t i = 0; i <= sibling->m_numKeys; ++i)
            child->m_children[i + m_minDegree] = sibling->m_children[i];
    }

    for (int32_t i = index + 1; i < m_numKeys; ++i)
        m_keys[i - 1] = m_keys[i];

    for (int32_t i = index + 2; i <= m_numKeys; ++i)
        m_children[i - 1] = m_children[i];

    child->m_numKeys += sibling->m_numKeys + 1;
    m_numKeys--;

    delete sibling;
}


// implementacion btree

BTree::BTree(short _minDegree) {
    m_root = nullptr;
    m_minDegree = _minDegree;
}

BTreeNode* BTree::getRoot() const {
    return m_root;
}

void BTree::traverse() {
    if (m_root != nullptr) {
        m_root->traverse();
    }
}

void BTree::insertPerson(uint32_t key) {
    if (m_root == nullptr) {
        m_root = new BTreeNode(m_minDegree, true);
        m_root->m_keys[0] = key; 
        m_root->m_numKeys = 1; 
    } else { 
        if (m_root->m_numKeys == 2 * m_minDegree - 1) {
            BTreeNode* newRoot = new BTreeNode(m_minDegree, false);

            newRoot->m_children[0] = m_root;
            newRoot->splitChild(0, m_root);

            int32_t index = 0;

            if (newRoot->m_keys[0] < key) 
                index++;

            newRoot->m_children[index]->insertNonFull(key);
            m_root = newRoot;
        } else { 
            m_root->insertNonFull(key);
        }
    }
}

bool BTree::searchDNI(uint32_t dni) {
    return (m_root == nullptr) ? false : m_root->search(dni);
}


void BTree::deleteDNI(uint32_t dni) {
    if (!m_root) {
        std::cout << "Árbol vacío!" << std::endl;
        return;
    }
    m_root->remove(dni);

    if (m_root->m_numKeys == 0) {
        BTreeNode* tmp = m_root;
        if (m_root->m_isLeaf)
            m_root = nullptr;
        else
            m_root = m_root->m_children[0];
        delete tmp;
    }
}

