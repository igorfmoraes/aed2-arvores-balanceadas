#ifndef LIVRO_ORDENS_AVL_H
#define LIVRO_ORDENS_AVL_H

#include <string>
#include <vector>
#include <map>

#include "ChaveOrdem.h"
#include "Ordem.h"
#include "AvlMap.h"
#include "HistoricoOrdens.h"

/**
 * Centraliza as operações do Livro de Ordens utilizando armazenamento em Árvore AVL.
 */
class LivroOrdensAVL {
private:
    AvlMap<ChaveOrdem, Ordem> activeOrdersTree;
    std::map<std::string, long> nextAvailableId;
    HistoricoOrdens operationsHistory;

    bool removeOrderInternal(std::string const& tickerSymbol, long orderIdentifier, OrderStatus finalStatus) {
        ChaveOrdem targetKey(tickerSymbol, orderIdentifier);

        Ordem* activeOrderPointer = activeOrdersTree.find(targetKey);
        if (activeOrderPointer == nullptr) {
            return false;
        }

        Ordem copiedOrder = *activeOrderPointer;
        bool wasRemoved = activeOrdersTree.erase(targetKey);

        if (wasRemoved) {
            operationsHistory.registrar(targetKey, copiedOrder, finalStatus);
        }

        return wasRemoved;
    }

public:
    LivroOrdensAVL() {}

    ChaveOrdem cadastrarOrdem(std::string const& tickerSymbol, TipoOrdem orderType, long quantityShares, double unitPrice) {
        long currentIdentifier = nextAvailableId[tickerSymbol];
        nextAvailableId[tickerSymbol] = currentIdentifier + 1;

        ChaveOrdem newKey(tickerSymbol, currentIdentifier);
        Ordem newOrder(currentIdentifier, tickerSymbol, orderType, quantityShares, unitPrice);

        activeOrdersTree.insert(newKey, newOrder);

        return newKey;
    }

    bool cancelarOrdem(std::string const& tickerSymbol, long orderIdentifier) {
        return removeOrderInternal(tickerSymbol, orderIdentifier, OrderStatus::CANCELADA);
    }

    bool executarOrdem(std::string const& tickerSymbol, long orderIdentifier) {
        return removeOrderInternal(tickerSymbol, orderIdentifier, OrderStatus::EXECUTADA);
    }

    HistoricoOrdens const& getHistorico() const {
        return operationsHistory;
    }

    Ordem* buscarOrdem(std::string const& tickerSymbol, long orderIdentifier) {
        ChaveOrdem searchKey(tickerSymbol, orderIdentifier);
        return activeOrdersTree.find(searchKey);
    }

    bool existeOrdem(std::string const& tickerSymbol, long orderIdentifier) {
        ChaveOrdem searchKey(tickerSymbol, orderIdentifier);
        return activeOrdersTree.contains(searchKey);
    }

    size_t quantidadeOrdens() const {
        return activeOrdersTree.size();
    }

    bool vazio() const {
        return activeOrdersTree.empty();
    }

    int alturaArvore() const {
        return activeOrdersTree.determineHeight();
    }

    void imprimirLivro() const {
        activeOrdersTree.printMap();
    }

    std::vector<ChaveOrdem> listarChaves() const {
        std::vector<ChaveOrdem> exportedKeys(activeOrdersTree.size());
        activeOrdersTree.getKeys(exportedKeys);
        return exportedKeys;
    }

    size_t contarOrdensPorTicker(std::string const& tickerSymbol) const {
        std::vector<ChaveOrdem> exportedKeys = listarChaves();
        size_t count = 0;
        for (ChaveOrdem const& key : exportedKeys) {
            if (key.getTicker() == tickerSymbol) {
                count++;
            }
        }
        return count;
    }

    std::map<std::string, size_t> contarOrdensPorTickerTodos() const {
        std::vector<ChaveOrdem> exportedKeys = listarChaves();
        std::map<std::string, size_t> aggregatedCounts;
        for (ChaveOrdem const& key : exportedKeys) {
            aggregatedCounts[key.getTicker()]++;
        }
        return aggregatedCounts;
    }

    size_t totalRotacoesSimplesEsquerdaEsquerda() const { return activeOrdersTree.getLeftLeftInsertRotations(); }
    size_t totalRotacoesDuplasEsquerdaDireita() const   { return activeOrdersTree.getLeftRightInsertRotations(); }
    size_t totalRotacoesDuplasDireitaEsquerda() const   { return activeOrdersTree.getRightLeftInsertRotations(); }
    size_t totalRotacoesSimplesDireitaDireita() const   { return activeOrdersTree.getRightRightInsertRotations(); }
    
    size_t totalRotacoesSimplesEsquerdaRemocao() const  { return activeOrdersTree.getLeftLeftRemoveRotations(); }
    size_t totalRotacoesDuplasEsquerdaRemocao() const   { return activeOrdersTree.getLeftRightRemoveRotations(); }
    size_t totalRotacoesDuplasDireitaRemocao() const    { return activeOrdersTree.getRightLeftRemoveRotations(); }
    size_t totalRotacoesSimplesDireitaRemocao() const   { return activeOrdersTree.getRightRightRemoveRotations(); }

    size_t totalRotacoes() const {
        return totalRotacoesSimplesEsquerdaEsquerda() + (totalRotacoesDuplasEsquerdaDireita() * 2) + 
               (totalRotacoesDuplasDireitaEsquerda() * 2) + totalRotacoesSimplesDireitaDireita() +
               totalRotacoesSimplesEsquerdaRemocao() + (totalRotacoesDuplasEsquerdaRemocao() * 2) + 
               (totalRotacoesDuplasDireitaRemocao() * 2) + totalRotacoesSimplesDireitaRemocao();
    }

    static size_t bytesPorNoEstimado() {
        return sizeof(ChaveOrdem) + sizeof(Ordem) + sizeof(int) + (2 * sizeof(void*));
    }

    size_t bytesTotaisEstimados() const {
        return quantidadeOrdens() * bytesPorNoEstimado();
    }
};

#endif
