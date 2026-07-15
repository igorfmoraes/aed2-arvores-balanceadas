import csv
import random
import os


def main():
    """
    Cria o diretório de workloads e coordena a criação dos respectivos arquivos CSV para os tamanhos definidos.
    """
    datasetSizes = [10000, 100000, 1000000]
    os.makedirs("workloads", exist_ok=True)

    for datasetSize in datasetSizes:
        searchHeavyWorkloadFilename = f"workloads/leitura_{datasetSize}.csv"
        searchHeavyGenerator = WorkloadGenerator(
            searchHeavyWorkloadFilename, datasetSize, 0.90, 0.05, 0.05
        )
        searchHeavyGenerator.generateAndSave()

        writeHeavyWorkloadFilename = f"workloads/escrita_{datasetSize}.csv"
        writeHeavyGenerator = WorkloadGenerator(
            writeHeavyWorkloadFilename, datasetSize, 0.10, 0.45, 0.45
        )
        writeHeavyGenerator.generateAndSave()


class WorkloadGenerator:
    """
    Constrói a tabela de operações contendo uma distribuição de busca, inserção e exclusão definidos pelos parâmetros.
    """

    def __init__(
        self,
        targetFilename,
        operationCount,
        searchPercentage,
        insertPercentage,
        deletePercentage,
    ):
        self.targetFilename = targetFilename
        self.operationCount = operationCount
        self.searchPercentage = searchPercentage
        self.insertPercentage = insertPercentage
        self.deletePercentage = deletePercentage
        self.marketPrice = 10000
        self.volatility = 1000
        self.activePrices = []

    def _processSingleOperation(self, operationType, fileWriter):
        if not self.activePrices:
            self._executeInsert(fileWriter)
            return

        if operationType == "Insert":
            self._executeInsert(fileWriter)
            return

        if operationType == "Search":
            self._executeSearch(fileWriter)
            return

        if operationType == "Delete":
            self._executeDelete(fileWriter)
            return

    def _executeInsert(self, fileWriter):
        price = round(random.gauss(self.marketPrice, self.volatility))
        self.activePrices.append(price)
        fileWriter.writerow(["Insert", price])

    def _executeSearch(self, fileWriter):
        price = random.choice(self.activePrices)
        fileWriter.writerow(["Search", price])

    def _executeDelete(self, fileWriter):
        targetIndex = random.randint(0, len(self.activePrices) - 1)
        price = self.activePrices[targetIndex]
        self.activePrices[targetIndex] = self.activePrices[-1]
        self.activePrices.pop()
        fileWriter.writerow(["Delete", price])

    def generateAndSave(self):
        operationsPool = self._createOperationsPool()

        with open(
            self.targetFilename, mode="w", newline="", encoding="utf-8"
        ) as fileHandle:
            fileWriter = csv.writer(fileHandle)
            fileWriter.writerow(["operation", "value"])

            for _ in range(self.operationCount):
                selectedOperation = random.choice(operationsPool)
                self._processSingleOperation(selectedOperation, fileWriter)

    def _createOperationsPool(self):
        searchCount = int(self.searchPercentage * 100)
        insertCount = int(self.insertPercentage * 100)
        deleteCount = int(self.deletePercentage * 100)
        return (
            (["Search"] * searchCount)
            + (["Insert"] * insertCount)
            + (["Delete"] * deleteCount)
        )


if __name__ == "__main__":
    main()
