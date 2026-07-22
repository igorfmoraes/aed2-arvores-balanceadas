#ifndef RED_BLACK_TREE_MAP_H
#define RED_BLACK_TREE_MAP_H

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

/**
 * Implementação completa de uma Árvore Rubro-Negra (Red-Black Tree) baseada em mapa.
 */
template <typename KeyType, typename ValueType>
class RbMap {
private:
    struct RbNode {
        KeyType key;
        ValueType value;
        bool isBlack;
        RbNode* parentNode;
        RbNode* leftChild;
        RbNode* rightChild;

        RbNode(KeyType const& k, ValueType const& v) 
            : key(k), value(v), isBlack(false), parentNode(nullptr), leftChild(nullptr), rightChild(nullptr) {}
    };

    RbNode* rootNode;
    size_t elementCount;

    int alturaNode(RbNode* node) const {
        if (node == nullptr) return 0;
        return 1 + std::max(alturaNode(node->leftChild), alturaNode(node->rightChild));
    }

    void rotateLeft(RbNode* x, bool isInsertion) {
        RbNode* y = x->rightChild;
        x->rightChild = y->leftChild;
        if (y->leftChild != nullptr) {
            y->leftChild->parentNode = x;
        }
        y->parentNode = x->parentNode;
        if (x->parentNode == nullptr) {
            rootNode = y;
        } else if (x == x->parentNode->leftChild) {
            x->parentNode->leftChild = y;
        } else {
            x->parentNode->rightChild = y;
        }
        y->leftChild = x;
        x->parentNode = y;

        if (isInsertion) {
            rotacoesEsquerdaInsercao++;
        } else {
            rotacoesEsquerdaRemocao++;
        }
    }

    void rotateRight(RbNode* y, bool isInsertion) {
        RbNode* x = y->leftChild;
        y->leftChild = x->rightChild;
        if (x->rightChild != nullptr) {
            x->rightChild->parentNode = y;
        }
        x->parentNode = y->parentNode;
        if (y->parentNode == nullptr) {
            rootNode = x;
        } else if (y == y->parentNode->rightChild) {
            y->parentNode->rightChild = x;
        } else {
            y->parentNode->leftChild = x;
        }
        x->rightChild = y;
        y->parentNode = x;

        if (isInsertion) {
            rotacoesDireitaInsercao++;
        } else {
            rotacoesDireitaRemocao++;
        }
    }

    void insertFixup(RbNode* z) {
        while (z != rootNode && z->parentNode->isBlack == false) {
            if (z->parentNode == z->parentNode->parentNode->leftChild) {
                RbNode* y = z->parentNode->parentNode->rightChild;
                if (y != nullptr && y->isBlack == false) {
                    z->parentNode->isBlack = true;
                    y->isBlack = true;
                    z->parentNode->parentNode->isBlack = false;
                    recoloracoesInsercao += 3;
                    z = z->parentNode->parentNode;
                } else {
                    if (z == z->parentNode->rightChild) {
                        z = z->parentNode;
                        rotateLeft(z, true);
                    }
                    z->parentNode->isBlack = true;
                    z->parentNode->parentNode->isBlack = false;
                    recoloracoesInsercao += 2;
                    rotateRight(z->parentNode->parentNode, true);
                }
            } else {
                RbNode* y = z->parentNode->parentNode->leftChild;
                if (y != nullptr && y->isBlack == false) {
                    z->parentNode->isBlack = true;
                    y->isBlack = true;
                    z->parentNode->parentNode->isBlack = false;
                    recoloracoesInsercao += 3;
                    z = z->parentNode->parentNode;
                } else {
                    if (z == z->parentNode->leftChild) {
                        z = z->parentNode;
                        rotateRight(z, true);
                    }
                    z->parentNode->isBlack = true;
                    z->parentNode->parentNode->isBlack = false;
                    recoloracoesInsercao += 2;
                    rotateLeft(z->parentNode->parentNode, true);
                }
            }
        }
        if (!rootNode->isBlack) {
            rootNode->isBlack = true;
            recoloracoesInsercao++;
        }
    }

    void transplant(RbNode* u, RbNode* v) {
        if (u->parentNode == nullptr) {
            rootNode = v;
        } else if (u == u->parentNode->leftChild) {
            u->parentNode->leftChild = v;
        } else {
            u->parentNode->rightChild = v;
        }
        if (v != nullptr) {
            v->parentNode = u->parentNode;
        }
    }

    RbNode* minimum(RbNode* node) const {
        while (node->leftChild != nullptr) {
            node = node->leftChild;
        }
        return node;
    }

    void deleteFixup(RbNode* x, RbNode* xParent) {
        while (x != rootNode && (x == nullptr || x->isBlack)) {
            RbNode* parent = (x != nullptr) ? x->parentNode : xParent;
            if (parent == nullptr) break;

            if (x == parent->leftChild) {
                RbNode* w = parent->rightChild;
                if (w != nullptr && !w->isBlack) {
                    w->isBlack = true;
                    parent->isBlack = false;
                    recoloracoesRemocao += 2;
                    rotateLeft(parent, false);
                    w = parent->rightChild;
                }
                if ((w->leftChild == nullptr || w->leftChild->isBlack) &&
                    (w->rightChild == nullptr || w->rightChild->isBlack)) {
                    if (w != nullptr) {
                        w->isBlack = false;
                        recoloracoesRemocao++;
                    }
                    x = parent;
                    xParent = x->parentNode;
                } else {
                    if (w->rightChild == nullptr || w->rightChild->isBlack) {
                        if (w->leftChild != nullptr) {
                            w->leftChild->isBlack = true;
                            recoloracoesRemocao++;
                        }
                        w->isBlack = false;
                        recoloracoesRemocao++;
                        rotateRight(w, false);
                        w = parent->rightChild;
                    }
                    if (w != nullptr) {
                        w->isBlack = parent->isBlack;
                        recoloracoesRemocao++;
                    }
                    parent->isBlack = true;
                    if (w->rightChild != nullptr) {
                        w->rightChild->isBlack = true;
                        recoloracoesRemocao++;
                    }
                    rotateLeft(parent, false);
                    x = rootNode;
                    break;
                }
            } else {
                RbNode* w = parent->leftChild;
                if (w != nullptr && !w->isBlack) {
                    w->isBlack = true;
                    parent->isBlack = false;
                    recoloracoesRemocao += 2;
                    rotateRight(parent, false);
                    w = parent->leftChild;
                }
                if ((w->rightChild == nullptr || w->rightChild->isBlack) &&
                    (w->leftChild == nullptr || w->leftChild->isBlack)) {
                    if (w != nullptr) {
                        w->isBlack = false;
                        recoloracoesRemocao++;
                    }
                    x = parent;
                    xParent = x->parentNode;
                } else {
                    if (w->leftChild == nullptr || w->leftChild->isBlack) {
                        if (w->rightChild != nullptr) {
                            w->rightChild->isBlack = true;
                            recoloracoesRemocao++;
                        }
                        w->isBlack = false;
                        recoloracoesRemocao++;
                        rotateLeft(w, false);
                        w = parent->leftChild;
                    }
                    if (w != nullptr) {
                        w->isBlack = parent->isBlack;
                        recoloracoesRemocao++;
                    }
                    parent->isBlack = true;
                    if (w->leftChild != nullptr) {
                        w->leftChild->isBlack = true;
                        recoloracoesRemocao++;
                    }
                    rotateRight(parent, false);
                    x = rootNode;
                    break;
                }
            }
        }
        if (x != nullptr) {
            x->isBlack = true;
            recoloracoesRemocao++;
        }
    }

    void clearRecursive(RbNode* node) {
        if (node == nullptr) return;
        clearRecursive(node->leftChild);
        clearRecursive(node->rightChild);
        delete node;
    }

    void getKeysInOrder(RbNode* node, std::vector<KeyType>& keysVector, size_t& index) const {
        if (node == nullptr) return;
        getKeysInOrder(node->leftChild, keysVector, index);
        if (index < keysVector.size()) {
            keysVector[index++] = node->key;
        }
        getKeysInOrder(node->rightChild, keysVector, index);
    }

    void printNode(RbNode* node, int indent) const {
        if (node == nullptr) return;
        printNode(node->rightChild, indent + 4);
        std::cout << std::string(indent, ' ') << node->key.toString() << (node->isBlack ? " (B)" : " (R)") << "\n";
        printNode(node->leftChild, indent + 4);
    }

public:
    size_t rotacoesEsquerdaInsercao;
    size_t rotacoesDireitaInsercao;
    size_t rotacoesEsquerdaRemocao;
    size_t rotacoesDireitaRemocao;
    size_t recoloracoesInsercao;
    size_t recoloracoesRemocao;

    RbMap() : rootNode(nullptr), elementCount(0),
              rotacoesEsquerdaInsercao(0), rotacoesDireitaInsercao(0),
              rotacoesEsquerdaRemocao(0), rotacoesDireitaRemocao(0),
              recoloracoesInsercao(0), recoloracoesRemocao(0) {}

    ~RbMap() { clear(); }

    size_t size() const { return elementCount; }
    
    bool empty() const { return elementCount == 0; }

    int altura() const {
        return alturaNode(rootNode);
    }

    bool contains(KeyType const& targetKey) const { return find(targetKey) != nullptr; }

    ValueType* find(KeyType const& targetKey) const {
        RbNode* currentNode = rootNode;
        while (currentNode != nullptr) {
            if (targetKey < currentNode->key) currentNode = currentNode->leftChild;
            else if (targetKey > currentNode->key) currentNode = currentNode->rightChild;
            else return &(currentNode->value);
        }
        return nullptr;
    }

    bool insert(KeyType const& targetKey, ValueType const& targetValue) {
        RbNode* y = nullptr;
        RbNode* x = rootNode;

        while (x != nullptr) {
            y = x;
            if (targetKey < x->key) {
                x = x->leftChild;
            } else if (targetKey > x->key) {
                x = x->rightChild;
            } else {
                x->value = targetValue;
                return false;
            }
        }

        RbNode* newNode = new RbNode(targetKey, targetValue);
        newNode->parentNode = y;
        if (y == nullptr) {
            rootNode = newNode;
        } else if (targetKey < y->key) {
            y->leftChild = newNode;
        } else {
            y->rightChild = newNode;
        }

        elementCount++;
        insertFixup(newNode);
        return true;
    }

    bool erase(KeyType const& targetKey) {
        RbNode* z = rootNode;
        while (z != nullptr) {
            if (targetKey < z->key) {
                z = z->leftChild;
            } else if (targetKey > z->key) {
                z = z->rightChild;
            } else {
                break;
            }
        }
        if (z == nullptr) return false;

        RbNode* y = z;
        RbNode* x = nullptr;
        RbNode* xParent = nullptr;
        bool yOriginalIsBlack = y->isBlack;

        if (z->leftChild == nullptr) {
            x = z->rightChild;
            transplant(z, z->rightChild);
            xParent = z->parentNode;
        } else if (z->rightChild == nullptr) {
            x = z->leftChild;
            transplant(z, z->leftChild);
            xParent = z->parentNode;
        } else {
            y = minimum(z->rightChild);
            yOriginalIsBlack = y->isBlack;
            x = y->rightChild;
            if (y->parentNode == z) {
                xParent = y;
            } else {
                xParent = y->parentNode;
                transplant(y, y->rightChild);
                y->rightChild = z->rightChild;
                y->rightChild->parentNode = y;
            }
            transplant(z, y);
            y->leftChild = z->leftChild;
            y->leftChild->parentNode = y;
            y->isBlack = z->isBlack;
            recoloracoesRemocao++;
        }

        delete z;
        elementCount--;

        if (yOriginalIsBlack) {
            deleteFixup(x, xParent);
        }

        return true;
    }

    void printMap() const {
        printNode(rootNode, 0);
    }

    void clear() {
        clearRecursive(rootNode);
        rootNode = nullptr;
        elementCount = 0;
    }

    void getKeys(std::vector<KeyType>& keysVector) const {
        keysVector.resize(elementCount);
        size_t index = 0;
        getKeysInOrder(rootNode, keysVector, index);
    }

    static size_t bytesPorNoEstimado() {
        return sizeof(KeyType) + sizeof(ValueType) + sizeof(bool) + (3 * sizeof(void*));
    }
};

#endif
