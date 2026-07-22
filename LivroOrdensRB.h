#ifndef LIVRO_ORDENS_RB_H
#define LIVRO_ORDENS_RB_H

#include <string>
#include <vector>
#include <map>

#include "ChaveOrdem.h"
#include "Ordem.h"
#include "RbMap.h"
#include "HistoricoOrdens.h"

/**
 * Centraliza as operações do Livro de Ordens utilizando armazenamento em Árvore Rubro-Negra.
 */
class LivroOrdensRB {
private:
    RbMap<ChaveOrdem, Ordem> activeOrdersTree;
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
    LivroOrdensRB() {}

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
        return activeOrdersTree.altura();
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

    size_t totalRotacoesEsquerdaInsercao() const { return activeOrdersTree.rotacoesEsquerdaInsercao; }
    size_t totalRotacoesDireitaInsercao() const  { return activeOrdersTree.rotacoesDireitaInsercao; }
    size_t totalRotacoesEsquerdaRemocao() const  { return activeOrdersTree.rotacoesEsquerdaRemocao; }
    size_t totalRotacoesDireitaRemocao() const   { return activeOrdersTree.rotacoesDireitaRemocao; }
    size_t totalRecoloracoesInsercao() const     { return activeOrdersTree.recoloracoesInsercao; }
    size_t totalRecoloracoesRemocao() const      { return activeOrdersTree.recoloracoesRemocao; }

    size_t totalRotacoes() const {
        return activeOrdersTree.rotacoesEsquerdaInsercao + activeOrdersTree.rotacoesDireitaInsercao
             + activeOrdersTree.rotacoesEsquerdaRemocao  + activeOrdersTree.rotacoesDireitaRemocao;
    }

    size_t totalRecoloracoes() const {
        return activeOrdersTree.recoloracoesInsercao + activeOrdersTree.recoloracoesRemocao;
    }

    static size_t bytesPorNoEstimado() {
        return RbMap<ChaveOrdem, Ordem>::bytesPorNoEstimado();
    }

    size_t bytesTotaisEstimados() const {
        return quantidadeOrdens() * bytesPorNoEstimado();
    }
};

#endif
