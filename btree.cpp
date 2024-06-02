#include "btree.h"
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
        std::cout << " " << m_keys[i]->dni;
    }
    if (!m_isLeaf) m_children[i]->traverse();
}

DatosPersona* BTreeNode::search(uint32_t dni) {
    uint32_t i = 0;
    while (i < m_numKeys && dni > m_keys[i]->dni) 
        i++;
    if (i < m_numKeys && m_keys[i]->dni == dni) 
        return m_keys[i];
    if (m_isLeaf) 
        return nullptr;
    return m_children[i]->search(dni);
}

void BTreeNode::insertNonFull(DatosPersona* person) {
    int32_t index = m_numKeys-1;
    if (m_isLeaf) {
        while (index >= 0 && m_keys[index]->dni > person->dni) {
            m_keys[index+1] = m_keys[index];
            index--;
        }
        m_keys[index+1] = person;
        m_numKeys++;
    } else { 
        while (index >= 0 && m_keys[index]->dni > person->dni) 
            index--;
        if (m_children[index+1]->m_numKeys == 2*m_minDegree-1) {
            splitChild(index+1, m_children[index+1]);
            if (m_keys[index+1]->dni < person->dni) index++;
        }
        m_children[index+1]->insertNonFull(person);
    }  
}

void BTreeNode::splitChild(int32_t index, BTreeNode* childNode) {
    BTreeNode* newChild = new BTreeNode(childNode->m_minDegree, childNode->m_isLeaf);
    newChild->m_numKeys = m_minDegree - 1;

    for (short j = 0; j < m_minDegree-1; j++){                          
        newChild->m_keys[j] = childNode->m_keys[j+m_minDegree];   
    }

    if (!childNode->m_isLeaf) {
        for (short j = 0; j < m_minDegree; j++) 
            newChild->m_children[j] = childNode->m_children[j+m_minDegree];
    }
    childNode->m_numKeys = m_minDegree - 1;

    for (short j = m_numKeys; j >= index+1; j--){
        m_children[j+1] = m_children[j];
    }
    m_children[index+1] = newChild;

    for (short j = m_numKeys-1; j >= index; j--){
        m_keys[j+1] = m_keys[j];
    }
    m_keys[index] = childNode->m_keys[m_minDegree-1];

    m_numKeys++;
}


void BTreeNode::remove(uint32_t dni) {
    int32_t index = 0;
    while (index < m_numKeys && m_keys[index]->dni < dni) 
        index++;

    if (index < m_numKeys && m_keys[index]->dni == dni) {
        if (m_isLeaf)
            removeFromLeaf(index);
        else
            removeFromNonLeaf(index);
    } else {
        if (m_isLeaf) {
            std::cout << "La llave " << dni << " no existe en el arbol\n";
            return;
        }

        bool flag = (index == m_numKeys);
        if (m_children[index]->m_numKeys < m_minDegree)
            fill(index);

        if (flag && index > m_numKeys)
            m_children[index - 1]->remove(dni);
        else
            m_children[index]->remove(dni);
    }
}

void BTreeNode::removeFromLeaf(int32_t index) {
    for (int32_t i = index + 1; i < m_numKeys; ++i)
        m_keys[i - 1] = m_keys[i];
    m_numKeys--;
}

void BTreeNode::removeFromNonLeaf(int32_t index) {
    DatosPersona* key = m_keys[index];

    if (m_children[index]->m_numKeys >= m_minDegree) {
        DatosPersona* pred = getPredecessor(index);
        m_keys[index] = pred;
        m_children[index]->remove(pred->dni);
    } else if (m_children[index + 1]->m_numKeys >= m_minDegree) {
        DatosPersona* succ = getSuccessor(index);
        m_keys[index] = succ;
        m_children[index + 1]->remove(succ->dni);
    } else {
        merge(index);
        m_children[index]->remove(key->dni);
    }
}

DatosPersona* BTreeNode::getPredecessor(int32_t index) {
    BTreeNode* cur = m_children[index];
    while (!cur->m_isLeaf)
        cur = cur->m_children[cur->m_numKeys];
    return cur->m_keys[cur->m_numKeys - 1];
}

DatosPersona* BTreeNode::getSuccessor(int32_t index) {
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

    for (uint32_t i = child->m_numKeys - 1; i >= 0; --i)
        child->m_keys[i + 1] = child->m_keys[i];

    if (!child->m_isLeaf) {
        for (uint32_t i = child->m_numKeys; i >= 0; --i)
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

    for (uint32_t i = 1; i < sibling->m_numKeys; ++i)
        sibling->m_keys[i - 1] = sibling->m_keys[i];

    if (!sibling->m_isLeaf) {
        for (uint32_t i = 1; i <= sibling->m_numKeys; ++i)
            sibling->m_children[i - 1] = sibling->m_children[i];
    }

    child->m_numKeys += 1;
    sibling->m_numKeys -= 1;
}

void BTreeNode::merge(int32_t index) {
    BTreeNode* child = m_children[index];
    BTreeNode* sibling = m_children[index + 1];

    child->m_keys[m_minDegree - 1] = m_keys[index];

    for (uint32_t i = 0; i < sibling->m_numKeys; ++i)
        child->m_keys[i + m_minDegree] = sibling->m_keys[i];

    if (!child->m_isLeaf) {
        for (uint32_t i = 0; i <= sibling->m_numKeys; ++i)
            child->m_children[i + m_minDegree] = sibling->m_children[i];
    }

    for (uint32_t i = index + 1; i < m_numKeys; ++i)
        m_keys[i - 1] = m_keys[i];

    for (uint32_t i = index + 2; i <= m_numKeys; ++i)
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

void BTree::insertPerson(DatosPersona* person) {
    if (m_root == nullptr) {
        m_root = new BTreeNode(m_minDegree, true);
        m_root->m_keys[0] = person; 
        m_root->m_numKeys = 1; 
    } else { 
        if (m_root->m_numKeys == 2*m_minDegree-1) {
            BTreeNode* newRoot = new BTreeNode(m_minDegree, false);

            newRoot->m_children[0] = m_root;
            newRoot->splitChild(0, m_root);

            int32_t index = 0;

            if (newRoot->m_keys[0]->dni < person->dni) 
                index++;

            newRoot->m_children[index]->insertNonFull(person);
            m_root = newRoot;
        } else { 
            m_root->insertNonFull(person);
        }
    }
}

DatosPersona* BTree::searchDNI(uint32_t dni) {
    return (m_root == nullptr) ? nullptr : m_root->search(dni);
}

void BTree::deleteDNI(uint32_t dni) {
    if (!m_root) {
        std::cout << "Arbol vacio!" << std::endl;
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

