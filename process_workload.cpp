#include "avlTree.h"
#include "burbTree.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/**
 * Representa uma única instrução para executar uma ordem do dataset, com um
 * tipo (Insere, Remove, Busca) e um valor.
 */
struct DatasetOrder {
  char operationType;
  uint32_t recordKey;
};

/**
 * Converte um arquivo de dataset em uma coleção de DatasetOrder a serem
 * executados.
 */
std::vector<DatasetOrder>
parseWorkloadData(const std::string &datasetFilename) {
  std::vector<DatasetOrder> extractedOrders;
  std::ifstream inputFile(datasetFilename);
  std::string currentLine;

  if (!inputFile.is_open()) {
    return extractedOrders;
  }

  std::getline(inputFile, currentLine);

  while (std::getline(inputFile, currentLine)) {
    if (currentLine.empty()) {
      continue;
    }

    std::stringstream lineStream(currentLine);
    std::string operationText;
    std::string recordKeyText;

    if (!std::getline(lineStream, operationText, ',')) {
      continue;
    }

    if (!std::getline(lineStream, recordKeyText, ',')) {
      continue;
    }

    DatasetOrder parsedOrder;
    parsedOrder.operationType = operationText[0];
    parsedOrder.recordKey = std::stoul(recordKeyText);
    extractedOrders.push_back(parsedOrder);
  }
  return extractedOrders;
}

template <typename SearchTree>
void executeSingleOrder(SearchTree &searchTree,
                        const DatasetOrder &orderToExecute) {
  if (orderToExecute.operationType == 'I') {
    searchTree.insert(orderToExecute.recordKey);
    return;
  }
  if (orderToExecute.operationType == 'D') {
    searchTree.erase(orderToExecute.recordKey);
    return;
  }
  if (orderToExecute.operationType == 'S') {
    searchTree.contains(orderToExecute.recordKey);
    return;
  }
}

template <typename SearchTree>
void processOrdersSequence(SearchTree &searchTree,
                           const std::vector<DatasetOrder> &ordersSequence,
                           size_t startIndex, size_t endIndex) {
  for (size_t orderIndex = startIndex; orderIndex < endIndex; ++orderIndex) {
    executeSingleOrder(searchTree, ordersSequence[orderIndex]);
  }
}

/**
 * Inicializa a árvore executando metade das ordens, depois processa a outra
 * metade enquanto mede o tempo.
 */
template <typename SearchTree>
double evaluateTreePerformanceNanoseconds(
    SearchTree &searchTree, const std::vector<DatasetOrder> &workloadOrders) {
  if (workloadOrders.empty()) {
    return 0.0;
  }

  size_t totalOrdersCount = workloadOrders.size();
  size_t warmupPhaseLimit = totalOrdersCount / 2;

  processOrdersSequence(searchTree, workloadOrders, 0, warmupPhaseLimit);

  auto executionStartTime = std::chrono::steady_clock::now();

  processOrdersSequence(searchTree, workloadOrders, warmupPhaseLimit,
                        totalOrdersCount);

  auto executionEndTime = std::chrono::steady_clock::now();
  auto totalDurationNanoseconds =
      std::chrono::duration_cast<std::chrono::nanoseconds>(executionEndTime -
                                                           executionStartTime);

  return static_cast<double>(totalDurationNanoseconds.count());
}

std::string extractScenarioPrefix(const std::string &fullFilePath) {
  size_t lastDirectorySlashIndex = fullFilePath.find_last_of('/');
  std::string filenameOnly = fullFilePath.substr(lastDirectorySlashIndex + 1);
  size_t underscoreSeparatorIndex = filenameOnly.find('_');
  return filenameOnly.substr(0, underscoreSeparatorIndex);
}

std::string extractDatasetSizeText(const std::string &fullFilePath) {
  size_t lastDirectorySlashIndex = fullFilePath.find_last_of('/');
  std::string filenameOnly = fullFilePath.substr(lastDirectorySlashIndex + 1);
  size_t underscoreSeparatorIndex = filenameOnly.find('_');
  size_t extensionDotIndex = filenameOnly.find('.');
  return filenameOnly.substr(underscoreSeparatorIndex + 1,
                             extensionDotIndex - underscoreSeparatorIndex - 1);
}

void evaluateBurbTree(std::ofstream &resultsFile,
                      const std::string &scenarioPrefix,
                      const std::string &datasetSizeText,
                      const std::vector<DatasetOrder> &workloadOrders) {
  burbTree<uint32_t> redBlackTree;
  redBlackTree.freedPreallocate(workloadOrders.size());

  double executionDurationNanoseconds =
      evaluateTreePerformanceNanoseconds(redBlackTree, workloadOrders);
  size_t totalTreeRotations = redBlackTree.rotateL + redBlackTree.rotateR;
  size_t consumedMemoryBytes = redBlackTree.size() * redBlackTree.nodeSize();

  resultsFile << scenarioPrefix << "," << datasetSizeText << ",Rubro Negra,"
              << static_cast<long long>(executionDurationNanoseconds) << ","
              << totalTreeRotations << "," << consumedMemoryBytes << std::endl;
}

void evaluateAvlTree(std::ofstream &resultsFile,
                     const std::string &scenarioPrefix,
                     const std::string &datasetSizeText,
                     const std::vector<DatasetOrder> &workloadOrders) {
  avlTree<uint32_t> heightBalancedTree;
  heightBalancedTree.freedPreallocate(workloadOrders.size());

  double executionDurationNanoseconds =
      evaluateTreePerformanceNanoseconds(heightBalancedTree, workloadOrders);
  size_t totalTreeRotations =
      heightBalancedTree.lli + (heightBalancedTree.lri * 2) +
      (heightBalancedTree.rli * 2) + heightBalancedTree.rri +
      heightBalancedTree.lle + (heightBalancedTree.lre * 2) +
      (heightBalancedTree.rle * 2) + heightBalancedTree.rre;
  size_t consumedMemoryBytes =
      heightBalancedTree.size() * heightBalancedTree.nodeSize();

  resultsFile << scenarioPrefix << "," << datasetSizeText << ",AVL,"
              << static_cast<long long>(executionDurationNanoseconds) << ","
              << totalTreeRotations << "," << consumedMemoryBytes << std::endl;
}

/**
 * Mapeia os passos para testar cada um dos cenários.
 */
int main(int argumentCount, char **argumentVector) {
  std::vector<std::string> targetWorkloadFiles = {
      "workloads/leitura_10000.csv",   "workloads/leitura_100000.csv",
      "workloads/leitura_1000000.csv", "workloads/escrita_10000.csv",
      "workloads/escrita_100000.csv",  "workloads/escrita_1000000.csv"};

  std::ofstream resultsFile("resultados.csv");
  if (!resultsFile.is_open()) {
    std::cerr << "Erro ao criar o arquivo resultados.csv" << std::endl;
    return 1;
  }

  resultsFile << "Cenario,Quantidade,Estrutura,Tempo_ns,Rotacoes,Memoria_bytes"
              << std::endl;

  for (const auto &targetFilePath : targetWorkloadFiles) {
    std::vector<DatasetOrder> workloadOrders =
        parseWorkloadData(targetFilePath);

    if (workloadOrders.empty()) {
      continue;
    }

    std::string scenarioPrefix = extractScenarioPrefix(targetFilePath);
    std::string datasetSizeText = extractDatasetSizeText(targetFilePath);

    evaluateBurbTree(resultsFile, scenarioPrefix, datasetSizeText,
                     workloadOrders);
    evaluateAvlTree(resultsFile, scenarioPrefix, datasetSizeText,
                    workloadOrders);
  }

  resultsFile.close();
  return 0;
}
