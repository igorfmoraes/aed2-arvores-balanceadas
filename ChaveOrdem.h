#ifndef CHAVE_ORDEM_H
#define CHAVE_ORDEM_H

#include <string>
#include <ostream>

/**
 * Representa a chave utilizada para indexar as ordens dentro das árvores.
 * Formada pela combinação (ticker + identifier).
 */
class ChaveOrdem {
private:
    std::string tickerSymbol;
    long orderIdentifier;

public:
    ChaveOrdem() : tickerSymbol(""), orderIdentifier(0) {}

    ChaveOrdem(std::string const& inputTicker, long inputIdentifier)
        : tickerSymbol(inputTicker), orderIdentifier(inputIdentifier) {}

    std::string getTicker() const {
        return tickerSymbol;
    }

    long getId() const {
        return orderIdentifier;
    }

    std::string toString() const {
        return tickerSymbol + "-" + std::to_string(orderIdentifier);
    }

    bool operator<(ChaveOrdem const& otherKey) const {
        if (tickerSymbol != otherKey.tickerSymbol) {
            return tickerSymbol < otherKey.tickerSymbol;
        }
        return orderIdentifier < otherKey.orderIdentifier;
    }

    bool operator>(ChaveOrdem const& otherKey) const {
        if (tickerSymbol != otherKey.tickerSymbol) {
            return tickerSymbol > otherKey.tickerSymbol;
        }
        return orderIdentifier > otherKey.orderIdentifier;
    }

    bool operator==(ChaveOrdem const& otherKey) const {
        return tickerSymbol == otherKey.tickerSymbol && orderIdentifier == otherKey.orderIdentifier;
    }

    bool operator!=(ChaveOrdem const& otherKey) const {
        return !(*this == otherKey);
    }
};

inline std::ostream& operator<<(std::ostream& outputStream, ChaveOrdem const& targetKey) {
    outputStream << targetKey.toString();
    return outputStream;
}

#endif
