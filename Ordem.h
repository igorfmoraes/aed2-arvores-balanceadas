#ifndef ORDEM_H
#define ORDEM_H

#include <string>
#include <sstream>
#include <iomanip>
#include <ostream>

/**
 * Define os tipos possíveis de uma ordem no sistema.
 */
enum class TipoOrdem {
    COMPRA,
    VENDA
};

inline std::string tipoOrdemParaTexto(TipoOrdem tipo) {
    return (tipo == TipoOrdem::COMPRA) ? "COMPRA" : "VENDA";
}

/**
 * Armazena os dados referentes a uma ordem de mercado específica.
 */
class Ordem {
private:
    long orderIdentifier;
    std::string tickerSymbol;
    TipoOrdem orderType;
    long quantityShares;
    double unitPrice;

public:
    Ordem() : orderIdentifier(0), tickerSymbol(""), orderType(TipoOrdem::COMPRA), quantityShares(0), unitPrice(0.0) {}

    Ordem(long inputIdentifier, std::string const& inputTicker, TipoOrdem inputType, long inputQuantity, double inputPrice)
        : orderIdentifier(inputIdentifier), tickerSymbol(inputTicker), orderType(inputType), quantityShares(inputQuantity), unitPrice(inputPrice) {}

    long getId() const { return orderIdentifier; }
    std::string getTicker() const { return tickerSymbol; }
    TipoOrdem getTipo() const { return orderType; }
    long getQuantidade() const { return quantityShares; }
    double getPreco() const { return unitPrice; }

    std::string toString() const {
        std::ostringstream textBuffer;
        textBuffer << tickerSymbol << "-" << orderIdentifier << " | "
                   << tipoOrdemParaTexto(orderType) << " | "
                   << "Qtd: " << quantityShares << " | "
                   << "Preco: " << std::fixed << std::setprecision(2) << unitPrice;
        return textBuffer.str();
    }
};

inline std::ostream& operator<<(std::ostream& outputStream, Ordem const& targetOrder) {
    outputStream << targetOrder.toString();
    return outputStream;
}

#endif
