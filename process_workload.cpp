#include "avlTree.h"
#include "burbTree.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Order {
  char operation;
  uint32_t key;
};

std::vector<Order> parseCSV(const std::string &filename) {
  std::vector<Order> orders;
  std::ifstream file(filename);
  std::string line;

  if (!file.is_open())
    return orders;

  std::getline(file, line);
  while (std::getline(file, line)) {
    if (line.empty())
      continue;
    std::stringstream ss(line);
    std::string opStr, keyStr;
    if (std::getline(ss, opStr, ',') && std::getline(ss, keyStr, ',')) {
      Order cmd;
      cmd.operation = opStr[0];
      cmd.key = std::stoul(keyStr);
      orders.push_back(cmd);
    }
  }
  return orders;
}

template <typename TreeType>
double processTree(TreeType &tree, const std::vector<Order> &orders) {
  if (orders.empty())
    return 0.0;

  size_t totalOrders = orders.size();
  size_t warmupLimit = totalOrders / 2;

  for (size_t i = 0; i < warmupLimit; ++i) {
    if (orders[i].operation == 'I')
      tree.insert(orders[i].key);
    else if (orders[i].operation == 'D')
      tree.erase(orders[i].key);
    else if (orders[i].operation == 'S')
      tree.contains(orders[i].key);
  }

  auto startTime = std::chrono::steady_clock::now();
  for (size_t i = warmupLimit; i < totalOrders; ++i) {
    if (orders[i].operation == 'I')
      tree.insert(orders[i].key);
    else if (orders[i].operation == 'D')
      tree.erase(orders[i].key);
    else if (orders[i].operation == 'S')
      tree.contains(orders[i].key);
  }
  auto endTime = std::chrono::steady_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
  return static_cast<double>(duration.count());
}

int main(int argc, char **argv) {
  std::vector<std::string> files = {
      "workloads/leitura_10000.csv",   "workloads/leitura_100000.csv",
      "workloads/leitura_1000000.csv", "workloads/escrita_10000.csv",
      "workloads/escrita_100000.csv",  "workloads/escrita_1000000.csv"};

  std::ofstream outFile("resultados.csv");
  if (!outFile.is_open()) {
    std::cerr << "Erro ao criar o arquivo resultados.csv" << std::endl;
    return 1;
  }

  outFile << "Cenario,Quantidade,Estrutura,Tempo_ns,Rotacoes,Memoria_bytes"
          << std::endl;

  for (const auto &file : files) {
    std::vector<Order> orders = parseCSV(file);
    if (orders.empty())
      continue;

    size_t lastSlash = file.find_last_of('/');
    std::string basename = file.substr(lastSlash + 1);
    size_t underscore = basename.find('_');
    size_t dot = basename.find('.');

    std::string scenario = basename.substr(0, underscore);
    std::string quantity =
        basename.substr(underscore + 1, dot - underscore - 1);

    // Teste BURB Tree
    {
      burbTree<uint32_t> bTree;
      bTree.freedPreallocate(orders.size());
      double timeNs = processTree(bTree, orders);
      size_t rotations = bTree.rotateL + bTree.rotateR;
      size_t mem = bTree.size() * bTree.nodeSize();

      outFile << scenario << "," << quantity << ",Rubro Negra,"
              << (long long)timeNs << "," << rotations << "," << mem
              << std::endl;
    }

    // Teste AVL Tree
    {
      avlTree<uint32_t> aTree;
      aTree.freedPreallocate(orders.size());
      double timeNs = processTree(aTree, orders);
      size_t rotations = aTree.lli + (aTree.lri * 2) + (aTree.rli * 2) +
                         aTree.rri + aTree.lle + (aTree.lre * 2) +
                         (aTree.rle * 2) + aTree.rre;
      size_t mem = aTree.size() * aTree.nodeSize();

      outFile << scenario << "," << quantity << ",AVL," << (long long)timeNs
              << "," << rotations << "," << mem << std::endl;
    }
  }

  outFile.close();
  return 0;
}
