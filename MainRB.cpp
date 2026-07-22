#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <fstream>

#include "LivroOrdensRB.h"

static const std::vector<std::string> BENCHMARK_TICKERS = {
    "PETR4", "VALE3", "ITUB4", "BBDC4", "ABEV3",
    "MGLU3", "WEGE3", "B3SA3", "RENT3", "SUZB3"
};

static void clearInputStreamBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static long readLongParameter(std::string const& messagePrompt) {
    long inputParameter;
    while (true) {
        std::cout << messagePrompt;
        if (std::cin >> inputParameter) {
            clearInputStreamBuffer();
            return inputParameter;
        }
        std::cout << "Valor invalido. Tente novamente.\n";
        clearInputStreamBuffer();
    }
}

static double readDoubleParameter(std::string const& messagePrompt) {
    double inputParameter;
    while (true) {
        std::cout << messagePrompt;
        if (std::cin >> inputParameter) {
            clearInputStreamBuffer();
            return inputParameter;
        }
        std::cout << "Valor invalido. Tente novamente.\n";
        clearInputStreamBuffer();
    }
}

static std::string readTextParameter(std::string const& messagePrompt) {
    std::string inputText;
    std::cout << messagePrompt;
    std::getline(std::cin, inputText);
    return inputText;
}

static TipoOrdem readOrderType() {
    while (true) {
        std::cout << "Tipo da ordem (1 = Compra, 2 = Venda): ";
        int optionIndex;
        if (std::cin >> optionIndex && (optionIndex == 1 || optionIndex == 2)) {
            clearInputStreamBuffer();
            return (optionIndex == 1) ? TipoOrdem::COMPRA : TipoOrdem::VENDA;
        }
        std::cout << "Opcao invalida. Digite 1 para Compra ou 2 para Venda.\n";
        clearInputStreamBuffer();
    }
}

static void registerOrderInteractively(LivroOrdensRB& orderBook) {
    std::string tickerSymbol = readTextParameter("Ticker da acao (ex: PETR4): ");
    TipoOrdem orderType = readOrderType();
    long quantityShares = readLongParameter("Quantidade: ");
    double unitPrice = readDoubleParameter("Preco unitario: ");

    auto timerStart = std::chrono::high_resolution_clock::now();
    ChaveOrdem activeKey = orderBook.cadastrarOrdem(tickerSymbol, orderType, quantityShares, unitPrice);
    auto timerEnd = std::chrono::high_resolution_clock::now();
    double durationMicroseconds = std::chrono::duration<double, std::micro>(timerEnd - timerStart).count();

    std::cout << "\nOrdem cadastrada com sucesso! Chave gerada: " << activeKey.toString() << "\n";
    std::cout << "Tempo de insercao: " << std::fixed << std::setprecision(3) << durationMicroseconds << " microssegundos\n";
}

static void cancelOrderInteractively(LivroOrdensRB& orderBook) {
    std::string tickerSymbol = readTextParameter("Ticker da ordem a cancelar: ");
    long orderIdentifier = readLongParameter("ID da ordem: ");

    auto timerStart = std::chrono::high_resolution_clock::now();
    bool isRemoved = orderBook.cancelarOrdem(tickerSymbol, orderIdentifier);
    auto timerEnd = std::chrono::high_resolution_clock::now();
    double durationMicroseconds = std::chrono::duration<double, std::micro>(timerEnd - timerStart).count();

    if (isRemoved) {
        std::cout << "\nOrdem " << tickerSymbol << "-" << orderIdentifier << " CANCELADA com sucesso.\n";
    } else {
        std::cout << "\nOrdem " << tickerSymbol << "-" << orderIdentifier << " nao foi encontrada.\n";
    }
    std::cout << "Tempo de remocao: " << std::fixed << std::setprecision(3) << durationMicroseconds << " microssegundos\n";
}

static void executeOrderInteractively(LivroOrdensRB& orderBook) {
    std::string tickerSymbol = readTextParameter("Ticker da ordem a executar: ");
    long orderIdentifier = readLongParameter("ID da ordem: ");

    auto timerStart = std::chrono::high_resolution_clock::now();
    bool isRemoved = orderBook.executarOrdem(tickerSymbol, orderIdentifier);
    auto timerEnd = std::chrono::high_resolution_clock::now();
    double durationMicroseconds = std::chrono::duration<double, std::micro>(timerEnd - timerStart).count();

    if (isRemoved) {
        std::cout << "\nOrdem " << tickerSymbol << "-" << orderIdentifier << " EXECUTADA com sucesso.\n";
    } else {
        std::cout << "\nOrdem " << tickerSymbol << "-" << orderIdentifier << " nao foi encontrada.\n";
    }
    std::cout << "Tempo de remocao: " << std::fixed << std::setprecision(3) << durationMicroseconds << " microssegundos\n";
}

static void displayOperationsHistory(LivroOrdensRB& orderBook) {
    HistoricoOrdens const& registeredHistory = orderBook.getHistorico();

    std::cout << "\n----- Historico de ordens (executadas/canceladas) -----\n";
    registeredHistory.imprimir();
    std::cout << "---------------------------------------------------------\n";
    std::cout << "Total de saidas registradas: " << registeredHistory.total() << "\n";
    std::cout << "  - Executadas: " << registeredHistory.totalExecutadas() << "\n";
    std::cout << "  - Canceladas: " << registeredHistory.totalCanceladas() << "\n";
}

static void searchOrderInteractively(LivroOrdensRB& orderBook) {
    std::string tickerSymbol = readTextParameter("Ticker da ordem a buscar: ");
    long orderIdentifier = readLongParameter("ID da ordem: ");

    auto timerStart = std::chrono::high_resolution_clock::now();
    Ordem* locatedOrder = orderBook.buscarOrdem(tickerSymbol, orderIdentifier);
    auto timerEnd = std::chrono::high_resolution_clock::now();
    double durationMicroseconds = std::chrono::duration<double, std::micro>(timerEnd - timerStart).count();

    if (locatedOrder != nullptr) {
        std::cout << "\nOrdem encontrada: " << *locatedOrder << "\n";
    } else {
        std::cout << "\nOrdem " << tickerSymbol << "-" << orderIdentifier << " nao encontrada no livro de ordens.\n";
    }
    std::cout << "Tempo de busca: " << std::fixed << std::setprecision(3) << durationMicroseconds << " microssegundos\n";
}

static void listAllActiveOrders(LivroOrdensRB& orderBook) {
    std::vector<ChaveOrdem> exportedKeys = orderBook.listarChaves();

    if (exportedKeys.empty()) {
        std::cout << "\nO livro de ordens esta vazio.\n";
        return;
    }

    std::cout << "\nOrdens cadastradas (" << exportedKeys.size() << "):\n";
    for (ChaveOrdem const& activeKey : exportedKeys) {
        std::cout << "  - " << activeKey.toString() << "\n";
    }
}

static void showTreeStatistics(LivroOrdensRB& orderBook) {
    std::cout << "\n----- Estatisticas da Arvore Rubro-Negra -----\n";
    std::cout << "Quantidade de ordens (nos): " << orderBook.quantidadeOrdens() << "\n";
    std::cout << "Altura da arvore:           " << orderBook.alturaArvore() << "\n";
    std::cout << "Rotacoes esquerda (insercao): " << orderBook.totalRotacoesEsquerdaInsercao() << "\n";
    std::cout << "Rotacoes direita  (insercao): " << orderBook.totalRotacoesDireitaInsercao() << "\n";
    std::cout << "Rotacoes esquerda (remocao):  " << orderBook.totalRotacoesEsquerdaRemocao() << "\n";
    std::cout << "Rotacoes direita  (remocao):  " << orderBook.totalRotacoesDireitaRemocao() << "\n";
    std::cout << "Total de rotacoes:            " << orderBook.totalRotacoes() << "\n";
    std::cout << "Recoloracoes (insercao):      " << orderBook.totalRecoloracoesInsercao() << "\n";
    std::cout << "Recoloracoes (remocao):       " << orderBook.totalRecoloracoesRemocao() << "\n";
    std::cout << "Total de recoloracoes:        " << orderBook.totalRecoloracoes() << "\n";
    std::cout << "Bytes estimados por no:     " << LivroOrdensRB::bytesPorNoEstimado() << " bytes\n";
    std::cout << "Memoria total estimada:     " << orderBook.bytesTotaisEstimados() << " bytes\n";
    std::cout << "---------------------------------------\n";
}

static void countOrdersByTickerInteractively(LivroOrdensRB& orderBook) {
    std::string tickerSymbol = readTextParameter("Ticker que deseja contar (ex: VALE3): ");

    auto timerStart = std::chrono::high_resolution_clock::now();
    size_t totalCount = orderBook.contarOrdensPorTicker(tickerSymbol);
    auto timerEnd = std::chrono::high_resolution_clock::now();
    double durationMicroseconds = std::chrono::duration<double, std::micro>(timerEnd - timerStart).count();

    std::cout << "\nO ticker " << tickerSymbol << " possui " << totalCount
              << " ordem(ns) cadastrada(s) no livro de ordens.\n";
    std::cout << "Tempo da consulta: " << std::fixed << std::setprecision(3)
              << durationMicroseconds << " microssegundos\n";
}

static void listOrderCountByTickerInteractively(LivroOrdensRB& orderBook) {
    std::map<std::string, size_t> orderCount = orderBook.contarOrdensPorTickerTodos();

    if (orderCount.empty()) {
        std::cout << "\nO livro de ordens esta vazio.\n";
        return;
    }

    std::cout << "\nQuantidade de ordens por ticker:\n";
    for (auto const& pair : orderCount) {
        std::cout << "  " << std::left << std::setw(8) << pair.first
                  << " -> " << pair.second << " ordem(ns)\n";
    }
}

static void insertRandomOrdersForBenchmark(LivroOrdensRB& orderBook, long ordersCount, std::mt19937& rngEngine, std::vector<ChaveOrdem>& insertedKeys) {
    std::uniform_int_distribution<size_t> indexDistribution(0, BENCHMARK_TICKERS.size() - 1);
    std::uniform_int_distribution<long> quantityDistribution(10, 1000);
    std::uniform_real_distribution<double> priceDistribution(5.0, 120.0);
    std::uniform_int_distribution<int> typeDistribution(0, 1);

    for (long orderIndex = 0; orderIndex < ordersCount; ++orderIndex) {
        std::string tickerSymbol = BENCHMARK_TICKERS[indexDistribution(rngEngine)];
        TipoOrdem activeType = (typeDistribution(rngEngine) == 0) ? TipoOrdem::COMPRA : TipoOrdem::VENDA;
        long quantityShares = quantityDistribution(rngEngine);
        double unitPrice = priceDistribution(rngEngine);

        ChaveOrdem insertedKey = orderBook.cadastrarOrdem(tickerSymbol, activeType, quantityShares, unitPrice);
        insertedKeys.push_back(insertedKey);
    }
}

static void runDetailedBenchmarkSequence(long ordersCount) {
    std::cout << "\n===== BENCHMARK DETALHADO: " << ordersCount << " ordens =====\n";

    {
        LivroOrdensRB orderBook;
        std::vector<ChaveOrdem> insertedKeys;
        insertedKeys.reserve(ordersCount);
        std::mt19937 rngEngine(42);

        auto timerStartInsert = std::chrono::high_resolution_clock::now();
        insertRandomOrdersForBenchmark(orderBook, ordersCount, rngEngine, insertedKeys);
        auto timerEndInsert = std::chrono::high_resolution_clock::now();
        double durationInsertMs = std::chrono::duration<double, std::milli>(timerEndInsert - timerStartInsert).count();

        std::cout << "\n--- Insercao ALEATORIA ---\n";
        std::cout << "Tempo total:            " << std::fixed << std::setprecision(3) << durationInsertMs << " ms\n";
        std::cout << "Altura da arvore:       " << orderBook.alturaArvore() << " (log2(n) = "
                  << std::log2((double)ordersCount) << ")\n";
        std::cout << "Total de rotacoes:      " << orderBook.totalRotacoes() << "\n";
        std::cout << "Memoria estimada:       " << orderBook.bytesTotaisEstimados() << " bytes\n";

        std::cout << "\nDistribuicao de ordens por ticker:\n";
        for (auto const& pair : orderBook.contarOrdensPorTickerTodos()) {
            std::cout << "  " << std::left << std::setw(8) << pair.first
                      << " -> " << pair.second << " ordem(ns)\n";
        }

        std::uniform_int_distribution<size_t> searchDistribution(0, insertedKeys.size() - 1);
        const long targetSearchCount = std::min<long>(ordersCount, 10000);

        auto timerStartSearch = std::chrono::high_resolution_clock::now();
        long successfulSearches = 0;
        for (long iterIndex = 0; iterIndex < targetSearchCount; ++iterIndex) {
            ChaveOrdem const& targetKey = insertedKeys[searchDistribution(rngEngine)];
            if (orderBook.existeOrdem(targetKey.getTicker(), targetKey.getId())) {
                ++successfulSearches;
            }
        }
        auto timerEndSearch = std::chrono::high_resolution_clock::now();
        double durationSearchMs = std::chrono::duration<double, std::milli>(timerEndSearch - timerStartSearch).count();

        std::cout << "\n--- Busca ---\n";
        std::cout << "Busca de " << targetSearchCount << " ordens concluida em "
                  << durationSearchMs << " ms (" << successfulSearches << " encontradas)\n";

        const long targetRemoveCount = ordersCount / 2;
        std::shuffle(insertedKeys.begin(), insertedKeys.end(), rngEngine);
        std::uniform_int_distribution<int> motiveDistribution(1, 100);

        size_t rotationsBefore = orderBook.totalRotacoes();
        auto timerStartRemove = std::chrono::high_resolution_clock::now();
        long successfulRemovals = 0;
        for (long iterIndex = 0; iterIndex < targetRemoveCount; ++iterIndex) {
            ChaveOrdem const& targetKey = insertedKeys[iterIndex];
            bool wasRemoved = false;
            if (motiveDistribution(rngEngine) <= 70) {
                wasRemoved = orderBook.executarOrdem(targetKey.getTicker(), targetKey.getId());
            } else {
                wasRemoved = orderBook.cancelarOrdem(targetKey.getTicker(), targetKey.getId());
            }
            if (wasRemoved) ++successfulRemovals;
        }
        auto timerEndRemove = std::chrono::high_resolution_clock::now();
        double durationRemoveMs = std::chrono::duration<double, std::milli>(timerEndRemove - timerStartRemove).count();

        std::cout << "\n--- Remocao (execucao + cancelamento misturados) ---\n";
        std::cout << "Remocao de " << targetRemoveCount << " ordens concluida em "
                  << durationRemoveMs << " ms (" << successfulRemovals << " removidas)\n";
        std::cout << "Altura da arvore apos remocao: " << orderBook.alturaArvore() << "\n";
        std::cout << "Rotacoes na remocao:            " << (orderBook.totalRotacoes() - rotationsBefore) << "\n";
        std::cout << "Quantidade final de ordens:     " << orderBook.quantidadeOrdens() << "\n";
        std::cout << "Historico -> executadas: " << orderBook.getHistorico().totalExecutadas()
                  << " | canceladas: " << orderBook.getHistorico().totalCanceladas() << "\n";
    }

    {
        LivroOrdensRB orderBook;
        auto timerStart = std::chrono::high_resolution_clock::now();
        for (long iterIndex = 0; iterIndex < ordersCount; ++iterIndex) {
            orderBook.cadastrarOrdem("PETR4", TipoOrdem::COMPRA, 100, 30.0);
        }
        auto timerEnd = std::chrono::high_resolution_clock::now();
        double durationMs = std::chrono::duration<double, std::milli>(timerEnd - timerStart).count();

        std::cout << "\n--- Insercao SEQUENCIAL (pior caso para BST sem balanceamento) ---\n";
        std::cout << "Tempo total:      " << std::fixed << std::setprecision(3) << durationMs << " ms\n";
        std::cout << "Altura da arvore: " << orderBook.alturaArvore() << " (log2(n) = "
                  << std::log2((double)ordersCount)
                  << " | uma BST comum chegaria a altura " << ordersCount << ")\n";
        std::cout << "Total de rotacoes: " << orderBook.totalRotacoes() << "\n";
    }

    std::cout << "\n===========================================\n";
}

static void displayBenchmarkMenu() {
    long quantityShares = readLongParameter("\nQuantidade de ordens para o teste de desempenho: ");
    if (quantityShares <= 0) {
        std::cout << "Quantidade invalida.\n";
        return;
    }
    runDetailedBenchmarkSequence(quantityShares);
}

static void runComparativeBenchmark() {
    std::vector<long> sizeList = { 5000, 25000, 50000, 250000, 500000, 1000000 };

    std::ofstream csvFile("resultados_rn.csv");
    csvFile << "arvore,n,tempo_insercao_ms,altura_apos_insercao,rotacoes_insercao,recoloracoes_insercao,"
            << "tempo_busca_ms,tempo_remocao_ms,altura_apos_remocao,rotacoes_remocao,recoloracoes_remocao,memoria_bytes\n";

    std::cout << "\n===== BENCHMARK COMPARATIVO (varios tamanhos) =====\n";
    std::cout << std::left
              << std::setw(10) << "n"
              << std::setw(16) << "insercao(ms)"
              << std::setw(10) << "altura"
              << std::setw(12) << "rotacoes"
              << std::setw(14) << "busca(ms)"
              << std::setw(16) << "remocao(ms)"
              << "\n";

    for (long n : sizeList) {
        LivroOrdensRB orderBook;
        std::vector<ChaveOrdem> insertedKeys;
        insertedKeys.reserve(n);
        std::mt19937 rngEngine(42);

        auto timerStartInsert = std::chrono::high_resolution_clock::now();
        insertRandomOrdersForBenchmark(orderBook, n, rngEngine, insertedKeys);
        auto timerEndInsert = std::chrono::high_resolution_clock::now();
        double durationInsertMs = std::chrono::duration<double, std::milli>(timerEndInsert - timerStartInsert).count();
        int heightAfterInsert = orderBook.alturaArvore();
        size_t rotationsInsert = orderBook.totalRotacoes();
        size_t recolorationsInsert = orderBook.totalRecoloracoes();

        std::uniform_int_distribution<size_t> searchDistribution(0, insertedKeys.size() - 1);
        long targetSearchCount = std::min<long>(n, 10000);
        auto timerStartSearch = std::chrono::high_resolution_clock::now();
        for (long iterIndex = 0; iterIndex < targetSearchCount; ++iterIndex) {
            ChaveOrdem const& targetKey = insertedKeys[searchDistribution(rngEngine)];
            orderBook.existeOrdem(targetKey.getTicker(), targetKey.getId());
        }
        auto timerEndSearch = std::chrono::high_resolution_clock::now();
        double durationSearchMs = std::chrono::duration<double, std::milli>(timerEndSearch - timerStartSearch).count();

        std::shuffle(insertedKeys.begin(), insertedKeys.end(), rngEngine);
        long targetRemoveCount = n / 2;
        auto timerStartRemove = std::chrono::high_resolution_clock::now();
        for (long iterIndex = 0; iterIndex < targetRemoveCount; ++iterIndex) {
            orderBook.cancelarOrdem(insertedKeys[iterIndex].getTicker(), insertedKeys[iterIndex].getId());
        }
        auto timerEndRemove = std::chrono::high_resolution_clock::now();
        double durationRemoveMs = std::chrono::duration<double, std::milli>(timerEndRemove - timerStartRemove).count();
        int heightAfterRemove = orderBook.alturaArvore();
        size_t rotationsRemove = orderBook.totalRotacoes() - rotationsInsert;
        size_t recolorationsRemove = orderBook.totalRecoloracoes() - recolorationsInsert;

        std::cout << std::left
                  << std::setw(10) << n
                  << std::setw(16) << std::fixed << std::setprecision(3) << durationInsertMs
                  << std::setw(10) << heightAfterInsert
                  << std::setw(12) << rotationsInsert
                  << std::setw(14) << durationSearchMs
                  << std::setw(16) << durationRemoveMs
                  << "\n";

        csvFile << "RN," << n << "," << durationInsertMs << "," << heightAfterInsert << ","
                << rotationsInsert << "," << recolorationsInsert << ","
                << durationSearchMs << "," << durationRemoveMs << ","
                << heightAfterRemove << "," << rotationsRemove << "," << recolorationsRemove << ","
                << orderBook.bytesTotaisEstimados() << "\n";
    }

    csvFile.close();
    std::cout << "\nResultados exportados para 'resultados_rn.csv'.\n";
    std::cout << "(para comparar com a AVL, rodem o mesmo benchmark na versao\n"
              << " que usa avlMap.h, que grava arvore=\"AVL\" no CSV, e comparem\n"
              << " as colunas dos dois arquivos nos graficos)\n";
    std::cout << "===========================================\n";
}

static void displayMenuOptions() {
    std::cout << "\n===== LIVRO DE ORDENS - ARVORE RUBRO-NEGRA =====\n"
              << "1  - Cadastrar ordem (insercao)\n"
              << "2  - Executar ordem (remocao por negocio fechado)\n"
              << "3  - Cancelar ordem (remocao por desistencia)\n"
              << "4  - Buscar ordem\n"
              << "5  - Listar ordens cadastradas\n"
              << "6  - Ver historico de ordens (executadas/canceladas)\n"
              << "7  - Imprimir estrutura da arvore\n"
              << "8  - Mostrar estatisticas gerais (altura, rotacoes, memoria)\n"
              << "9  - Contar ordens de um ticker especifico (ex: quantas da VALE3)\n"
              << "10 - Listar contagem de ordens por ticker (todos)\n"
              << "11 - Benchmark detalhado (aleatorio x sequencial, insercao/busca/remocao)\n"
              << "12 - Benchmark comparativo em varios tamanhos (gera resultados_rn.csv)\n"
              << "0  - Sair\n"
              << "Escolha uma opcao: ";
}

int main() {
    LivroOrdensRB internalOrderBook;
    int selectionIndex = -1;

    std::cout << "Bem-vindo ao sistema simplificado de Livro de Ordens.\n"
              << "Este programa demonstra o uso de uma Arvore Rubro-Negra\n"
              << "para gerenciar ordens de compra e venda de acoes.\n";

    while (selectionIndex != 0) {
        displayMenuOptions();

        if (!(std::cin >> selectionIndex)) {
            clearInputStreamBuffer();
            continue;
        }
        clearInputStreamBuffer();

        switch (selectionIndex) {
            case 1: registerOrderInteractively(internalOrderBook); break;
            case 2: executeOrderInteractively(internalOrderBook); break;
            case 3: cancelOrderInteractively(internalOrderBook); break;
            case 4: searchOrderInteractively(internalOrderBook); break;
            case 5: listAllActiveOrders(internalOrderBook); break;
            case 6: displayOperationsHistory(internalOrderBook); break;
            case 7: 
                std::cout << "\nEstrutura da arvore (raiz a esquerda, folhas a direita):\n";
                internalOrderBook.imprimirLivro(); 
                break;
            case 8: showTreeStatistics(internalOrderBook); break;
            case 9: countOrdersByTickerInteractively(internalOrderBook); break;
            case 10: listOrderCountByTickerInteractively(internalOrderBook); break;
            case 11: displayBenchmarkMenu(); break;
            case 12: runComparativeBenchmark(); break;
            case 0: 
                std::cout << "\nEncerrando o sistema. Ate mais!\n";
                break;
            default: 
                std::cout << "\nOpcao invalida. Tente novamente.\n";
                break;
        }
    }

    return 0;
}
