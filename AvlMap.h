#ifndef ADELSON_VELSKII_LANDIS_WIRTH_AVL_MAP_RECURSE_H
#define ADELSON_VELSKII_LANDIS_WIRTH_AVL_MAP_RECURSE_H

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * Representa o estado atual das operações de modificação na árvore,
 * permitindo propagar informações sobre balanceamento e atualizações
 * pela pilha de recursão.
 */
struct TreeState {
    bool heightChanged = false;
    bool nodeWasUpdated = false;
    bool nodeWasRemoved = false;
    size_t leftLeftInsertRotations = 0;
    size_t leftRightInsertRotations = 0;
    size_t rightLeftInsertRotations = 0;
    size_t rightRightInsertRotations = 0;
    size_t leftLeftRemoveRotations = 0;
    size_t leftRightRemoveRotations = 0;
    size_t rightLeftRemoveRotations = 0;
    size_t rightRightRemoveRotations = 0;
};

/**
 * Mapa balanceado implementado por meio de uma Árvore AVL genérica.
 */
template <typename KeyType, typename ValueType>
class AvlMap {
private:
    struct AvlNode {
        KeyType key;
        ValueType value;
        int balanceFactor;
        AvlNode* leftChild;
        AvlNode* rightChild;

        AvlNode(KeyType const& nodeKey, ValueType const& nodeValue, TreeState& state) {
            state.heightChanged = true;
            key = nodeKey;
            value = nodeValue;
            balanceFactor = 0;
            leftChild = nullptr;
            rightChild = nullptr;
        }
    };

    AvlNode* rootNode;
    size_t elementCount;
    TreeState treeState;

    void clearRecursive(AvlNode* node) {
        if (node == nullptr) {
            return;
        }
        clearRecursive(node->leftChild);
        clearRecursive(node->rightChild);
        delete node;
    }

    int calculateHeightRecursive(AvlNode* node) const {
        if (node == nullptr) {
            return 0;
        }
        int leftHeight = calculateHeightRecursive(node->leftChild);
        int rightHeight = calculateHeightRecursive(node->rightChild);
        return 1 + std::max(leftHeight, rightHeight);
    }

    bool containsRecursive(AvlNode* node, KeyType const& targetKey) const {
        return findRecursive(node, targetKey) != nullptr;
    }

    ValueType* findRecursive(AvlNode* node, KeyType const& targetKey) const {
        AvlNode* currentNode = node;
        while (currentNode != nullptr) {
            if (targetKey < currentNode->key) {
                currentNode = currentNode->leftChild;
            } else if (targetKey > currentNode->key) {
                currentNode = currentNode->rightChild;
            } else {
                return &(currentNode->value);
            }
        }
        return nullptr;
    }

    AvlNode* performLeftLeftInsertRotation(AvlNode* parentNode, AvlNode* leftChildNode) {
        treeState.leftLeftInsertRotations++;
        parentNode->leftChild = leftChildNode->rightChild;
        leftChildNode->rightChild = parentNode;
        parentNode->balanceFactor = 0;
        return leftChildNode;
    }

    AvlNode* performLeftRightInsertRotation(AvlNode* parentNode, AvlNode* leftChildNode) {
        treeState.leftRightInsertRotations++;
        AvlNode* rightGrandChild = leftChildNode->rightChild;
        leftChildNode->rightChild = rightGrandChild->leftChild;
        rightGrandChild->leftChild = leftChildNode;
        parentNode->leftChild = rightGrandChild->rightChild;
        rightGrandChild->rightChild = parentNode;

        parentNode->balanceFactor = (rightGrandChild->balanceFactor == -1) ? 1 : 0;
        leftChildNode->balanceFactor = (rightGrandChild->balanceFactor == 1) ? -1 : 0;
        
        return rightGrandChild;
    }

    AvlNode* balanceAfterLeftInsertion(AvlNode* node) {
        if (!treeState.heightChanged) {
            return node;
        }
        if (node->balanceFactor == 1) {
            node->balanceFactor = 0;
            treeState.heightChanged = false;
            return node;
        }
        if (node->balanceFactor == 0) {
            node->balanceFactor = -1;
            return node;
        }

        AvlNode* leftChild = node->leftChild;
        if (leftChild->balanceFactor == -1) {
            node = performLeftLeftInsertRotation(node, leftChild);
        } else {
            node = performLeftRightInsertRotation(node, leftChild);
        }
        node->balanceFactor = 0;
        treeState.heightChanged = false;
        return node;
    }

    AvlNode* performRightRightInsertRotation(AvlNode* parentNode, AvlNode* rightChildNode) {
        treeState.rightRightInsertRotations++;
        parentNode->rightChild = rightChildNode->leftChild;
        rightChildNode->leftChild = parentNode;
        parentNode->balanceFactor = 0;
        return rightChildNode;
    }

    AvlNode* performRightLeftInsertRotation(AvlNode* parentNode, AvlNode* rightChildNode) {
        treeState.rightLeftInsertRotations++;
        AvlNode* leftGrandChild = rightChildNode->leftChild;
        rightChildNode->leftChild = leftGrandChild->rightChild;
        leftGrandChild->rightChild = rightChildNode;
        parentNode->rightChild = leftGrandChild->leftChild;
        leftGrandChild->leftChild = parentNode;

        parentNode->balanceFactor = (leftGrandChild->balanceFactor == 1) ? -1 : 0;
        rightChildNode->balanceFactor = (leftGrandChild->balanceFactor == -1) ? 1 : 0;
        
        return leftGrandChild;
    }

    AvlNode* balanceAfterRightInsertion(AvlNode* node) {
        if (!treeState.heightChanged) {
            return node;
        }
        if (node->balanceFactor == -1) {
            node->balanceFactor = 0;
            treeState.heightChanged = false;
            return node;
        }
        if (node->balanceFactor == 0) {
            node->balanceFactor = 1;
            return node;
        }

        AvlNode* rightChild = node->rightChild;
        if (rightChild->balanceFactor == 1) {
            node = performRightRightInsertRotation(node, rightChild);
        } else {
            node = performRightLeftInsertRotation(node, rightChild);
        }
        node->balanceFactor = 0;
        treeState.heightChanged = false;
        return node;
    }

    AvlNode* insertRecursive(AvlNode* node, KeyType const& targetKey, ValueType const& targetValue) {
        if (node == nullptr) {
            treeState.nodeWasUpdated = false;
            return new AvlNode(targetKey, targetValue, treeState);
        }
        if (targetKey == node->key) {
            node->value = targetValue;
            treeState.heightChanged = false;
            treeState.nodeWasUpdated = true;
            return node;
        }
        if (targetKey < node->key) {
            node->leftChild = insertRecursive(node->leftChild, targetKey, targetValue);
            return balanceAfterLeftInsertion(node);
        }
        node->rightChild = insertRecursive(node->rightChild, targetKey, targetValue);
        return balanceAfterRightInsertion(node);
    }

    AvlNode* performRightRightRemoveRotation(AvlNode* parentNode, AvlNode* rightChildNode) {
        treeState.rightRightRemoveRotations++;
        parentNode->rightChild = rightChildNode->leftChild;
        rightChildNode->leftChild = parentNode;
        if (rightChildNode->balanceFactor == 0) {
            parentNode->balanceFactor = 1;
            rightChildNode->balanceFactor = -1;
            treeState.heightChanged = false;
        } else {
            parentNode->balanceFactor = 0;
            rightChildNode->balanceFactor = 0;
        }
        return rightChildNode;
    }

    AvlNode* performRightLeftRemoveRotation(AvlNode* parentNode, AvlNode* rightChildNode) {
        treeState.rightLeftRemoveRotations++;
        AvlNode* leftGrandChild = rightChildNode->leftChild;
        rightChildNode->leftChild = leftGrandChild->rightChild;
        leftGrandChild->rightChild = rightChildNode;
        parentNode->rightChild = leftGrandChild->leftChild;
        leftGrandChild->leftChild = parentNode;

        parentNode->balanceFactor = (leftGrandChild->balanceFactor == 1) ? -1 : 0;
        rightChildNode->balanceFactor = (leftGrandChild->balanceFactor == -1) ? 1 : 0;
        
        leftGrandChild->balanceFactor = 0;
        return leftGrandChild;
    }

    AvlNode* balanceAfterLeftDeletion(AvlNode* node) {
        if (node->balanceFactor == -1) {
            node->balanceFactor = 0;
            return node;
        }
        if (node->balanceFactor == 0) {
            node->balanceFactor = 1;
            treeState.heightChanged = false;
            return node;
        }
        AvlNode* rightChild = node->rightChild;
        if (rightChild->balanceFactor >= 0) {
            return performRightRightRemoveRotation(node, rightChild);
        }
        return performRightLeftRemoveRotation(node, rightChild);
    }

    AvlNode* performLeftLeftRemoveRotation(AvlNode* parentNode, AvlNode* leftChildNode) {
        treeState.leftLeftRemoveRotations++;
        parentNode->leftChild = leftChildNode->rightChild;
        leftChildNode->rightChild = parentNode;
        if (leftChildNode->balanceFactor == 0) {
            parentNode->balanceFactor = -1;
            leftChildNode->balanceFactor = 1;
            treeState.heightChanged = false;
        } else {
            parentNode->balanceFactor = 0;
            leftChildNode->balanceFactor = 0;
        }
        return leftChildNode;
    }

    AvlNode* performLeftRightRemoveRotation(AvlNode* parentNode, AvlNode* leftChildNode) {
        treeState.leftRightRemoveRotations++;
        AvlNode* rightGrandChild = leftChildNode->rightChild;
        leftChildNode->rightChild = rightGrandChild->leftChild;
        rightGrandChild->leftChild = leftChildNode;
        parentNode->leftChild = rightGrandChild->rightChild;
        rightGrandChild->rightChild = parentNode;

        parentNode->balanceFactor = (rightGrandChild->balanceFactor == -1) ? 1 : 0;
        leftChildNode->balanceFactor = (rightGrandChild->balanceFactor == 1) ? -1 : 0;
        
        rightGrandChild->balanceFactor = 0;
        return rightGrandChild;
    }

    AvlNode* balanceAfterRightDeletion(AvlNode* node) {
        if (node->balanceFactor == 1) {
            node->balanceFactor = 0;
            return node;
        }
        if (node->balanceFactor == 0) {
            node->balanceFactor = -1;
            treeState.heightChanged = false;
            return node;
        }
        AvlNode* leftChild = node->leftChild;
        if (leftChild->balanceFactor <= 0) {
            return performLeftLeftRemoveRotation(node, leftChild);
        }
        return performLeftRightRemoveRotation(node, leftChild);
    }

    AvlNode* eraseLeftmostChild(AvlNode* node, AvlNode*& nodeToRemove) {
        if (node->leftChild != nullptr) {
            node->leftChild = eraseLeftmostChild(node->leftChild, nodeToRemove);
            if (treeState.heightChanged) {
                return balanceAfterLeftDeletion(node);
            }
            return node;
        }
        nodeToRemove->key = node->key;
        nodeToRemove->value = node->value;
        nodeToRemove = node;
        AvlNode* rightChild = node->rightChild;
        treeState.heightChanged = true;
        return rightChild;
    }

    AvlNode* eraseRightmostChild(AvlNode* node, AvlNode*& nodeToRemove) {
        if (node->rightChild != nullptr) {
            node->rightChild = eraseRightmostChild(node->rightChild, nodeToRemove);
            if (treeState.heightChanged) {
                return balanceAfterRightDeletion(node);
            }
            return node;
        }
        nodeToRemove->key = node->key;
        nodeToRemove->value = node->value;
        nodeToRemove = node;
        AvlNode* leftChild = node->leftChild;
        treeState.heightChanged = true;
        return leftChild;
    }

    AvlNode* processNodeDeletion(AvlNode* node) {
        AvlNode* targetNode = node;
        if (node->rightChild == nullptr) {
            AvlNode* leftChild = node->leftChild;
            treeState.heightChanged = true;
            delete targetNode;
            treeState.nodeWasRemoved = true;
            return leftChild;
        }
        if (node->leftChild == nullptr) {
            AvlNode* rightChild = node->rightChild;
            treeState.heightChanged = true;
            delete targetNode;
            treeState.nodeWasRemoved = true;
            return rightChild;
        }
        
        if (node->balanceFactor <= 0) {
            node->leftChild = eraseRightmostChild(node->leftChild, targetNode);
            if (treeState.heightChanged) {
                node = balanceAfterLeftDeletion(node);
            }
        } else {
            node->rightChild = eraseLeftmostChild(node->rightChild, targetNode);
            if (treeState.heightChanged) {
                node = balanceAfterRightDeletion(node);
            }
        }
        delete targetNode;
        treeState.nodeWasRemoved = true;
        return node;
    }

    AvlNode* eraseRecursive(AvlNode* node, KeyType const& targetKey) {
        if (node == nullptr) {
            treeState.heightChanged = false;
            treeState.nodeWasRemoved = false;
            return nullptr;
        }
        if (targetKey < node->key) {
            node->leftChild = eraseRecursive(node->leftChild, targetKey);
            if (treeState.heightChanged) {
                return balanceAfterLeftDeletion(node);
            }
            return node;
        }
        if (targetKey > node->key) {
            node->rightChild = eraseRecursive(node->rightChild, targetKey);
            if (treeState.heightChanged) {
                return balanceAfterRightDeletion(node);
            }
            return node;
        }
        return processNodeDeletion(node);
    }

    void printMapRecursive(AvlNode* node, int depthLevel) const {
        if (node == nullptr) {
            return;
        }
        printMapRecursive(node->rightChild, depthLevel + 1);
        for (int index = 0; index < depthLevel; ++index) {
            std::cout << "    ";
        }
        std::cout << node->key << "\n";
        printMapRecursive(node->leftChild, depthLevel + 1);
    }

    void getKeysRecursive(AvlNode* node, std::vector<KeyType>& keysVector, size_t& currentIndex) const {
        if (node == nullptr) {
            return;
        }
        getKeysRecursive(node->leftChild, keysVector, currentIndex);
        keysVector[currentIndex++] = node->key;
        getKeysRecursive(node->rightChild, keysVector, currentIndex);
    }

public:
    AvlMap() {
        rootNode = nullptr;
        elementCount = 0;
    }

    ~AvlMap() {
        clear();
    }

    size_t size() const {
        return elementCount;
    }

    bool empty() const {
        return elementCount == 0;
    }

    int determineHeight() const {
        return calculateHeightRecursive(rootNode);
    }

    bool contains(KeyType const& targetKey) const {
        return containsRecursive(rootNode, targetKey);
    }

    ValueType* find(KeyType const& targetKey) const {
        return findRecursive(rootNode, targetKey);
    }

    bool insert(KeyType const& targetKey, ValueType const& targetValue) {
        treeState.heightChanged = false;
        treeState.nodeWasUpdated = false;
        
        rootNode = insertRecursive(rootNode, targetKey, targetValue);
        
        if (!treeState.nodeWasUpdated) {
            elementCount++;
        }
        return treeState.nodeWasUpdated;
    }

    bool erase(KeyType const& targetKey) {
        treeState.heightChanged = false;
        treeState.nodeWasRemoved = false;
        
        rootNode = eraseRecursive(rootNode, targetKey);
        
        if (treeState.nodeWasRemoved) {
            elementCount--;
        }
        return treeState.nodeWasRemoved;
    }

    void printMap() const {
        printMapRecursive(rootNode, 0);
    }

    void clear() {
        clearRecursive(rootNode);
        rootNode = nullptr;
        elementCount = 0;
    }

    void getKeys(std::vector<KeyType>& keysVector) const {
        if (rootNode != nullptr) {
            size_t currentIndex = 0;
            getKeysRecursive(rootNode, keysVector, currentIndex);
        }
    }

    size_t getLeftLeftInsertRotations() const { return treeState.leftLeftInsertRotations; }
    size_t getLeftRightInsertRotations() const { return treeState.leftRightInsertRotations; }
    size_t getRightLeftInsertRotations() const { return treeState.rightLeftInsertRotations; }
    size_t getRightRightInsertRotations() const { return treeState.rightRightInsertRotations; }
    size_t getLeftLeftRemoveRotations() const { return treeState.leftLeftRemoveRotations; }
    size_t getLeftRightRemoveRotations() const { return treeState.leftRightRemoveRotations; }
    size_t getRightLeftRemoveRotations() const { return treeState.rightLeftRemoveRotations; }
    size_t getRightRightRemoveRotations() const { return treeState.rightRightRemoveRotations; }
};

#endif
