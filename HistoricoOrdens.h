#ifndef HISTORICO_ORDENS_H
#define HISTORICO_ORDENS_H

#include <string>
#include <vector>
#include <ostream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>

#include "ChaveOrdem.h"
#include "Ordem.h"

/**
 * Identifica o motivo pelo qual uma ordem foi registrada no histórico.
 */
enum class OrderStatus {
    EXECUTADA,
    CANCELADA
};

inline std::string statusHistoricoParaTexto(OrderStatus status) {
    return (status == OrderStatus::EXECUTADA) ? "EXECUTADA" : "CANCELADA";
}

/**
 * Mantém uma fotografia imutável da ordem no momento da finalização.
 */
class RegistroHistorico {
private:
    ChaveOrdem orderKey;
    Ordem orderData;
    OrderStatus finalStatus;
    std::chrono::system_clock::time_point eventTimestamp;

public:
    RegistroHistorico() : finalStatus(OrderStatus::CANCELADA) {}

    RegistroHistorico(ChaveOrdem const& inputKey, Ordem const& inputOrder, OrderStatus inputStatus)
        : orderKey(inputKey), orderData(inputOrder), finalStatus(inputStatus),
          eventTimestamp(std::chrono::system_clock::now()) {}

    ChaveOrdem getChave() const { return orderKey; }
    Ordem getOrdem() const { return orderData; }
    OrderStatus getStatus() const { return finalStatus; }

    std::string toString() const {
        std::ostringstream textBuffer;
        std::time_t standardTime = std::chrono::system_clock::to_time_t(eventTimestamp);
        char timeBuffer[16];
        std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", std::localtime(&standardTime));
        
        textBuffer << "[" << timeBuffer << "] "
                   << orderKey.toString() << " - "
                   << statusHistoricoParaTexto(finalStatus) << " - "
                   << orderData.toString();
        return textBuffer.str();
    }
};

/**
 * Log cronológico de todas as ordens removidas.
 */
class HistoricoOrdens {
private:
    std::vector<RegistroHistorico> chronologicalRecords;

public:
    void registrar(ChaveOrdem const& targetKey, Ordem const& targetOrder, OrderStatus targetStatus) {
        chronologicalRecords.emplace_back(targetKey, targetOrder, targetStatus);
    }

    size_t total() const { 
        return chronologicalRecords.size(); 
    }

    size_t countByStatus(OrderStatus targetStatus) const {
        size_t count = 0;
        for (RegistroHistorico const& record : chronologicalRecords) {
            if (record.getStatus() == targetStatus) {
                count++;
            }
        }
        return count;
    }

    size_t totalExecutadas() const {
        return countByStatus(OrderStatus::EXECUTADA);
    }

    size_t totalCanceladas() const {
        return countByStatus(OrderStatus::CANCELADA);
    }

    std::vector<RegistroHistorico> const& listar() const {
        return chronologicalRecords;
    }

    std::vector<RegistroHistorico> listarPorTicker(std::string const& tickerSymbol) const {
        std::vector<RegistroHistorico> filteredResults;
        for (RegistroHistorico const& record : chronologicalRecords) {
            if (record.getChave().getTicker() == tickerSymbol) {
                filteredResults.push_back(record);
            }
        }
        return filteredResults;
    }

    void imprimir() const {
        if (chronologicalRecords.empty()) {
            std::cout << "Historico vazio: nenhuma ordem foi executada ou cancelada ainda.\n";
            return;
        }
        for (RegistroHistorico const& record : chronologicalRecords) {
            std::cout << record.toString() << "\n";
        }
    }
};

#endif
