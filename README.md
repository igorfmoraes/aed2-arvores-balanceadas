# Análise Comparativa de Árvores Balanceadas em Sistemas Compra e Vendar de Ações (High Frequency Trading)
Estudo de caso desenvolvido para a disciplina de **Algoritmos e Estruturas de Dados II** ministrada pelo professor Carlo Kléber na UFABC, com o objetivo de avaliar o desempenho prático de duas árvores balanceadas, **AVL** e **Rubro-Negra**, aplicadas a um **Livro de Ofertas (*Order Book*)** similar aos utilizados em plataformas de investimentos.

## Livro de Ofertas

Em bolsas de valores e sistemas de HFT (negociação de alta frequência automatizada por computadores), milhares de ordens de compra e venda de ações chegam a cada segundo. 
O Livro de Orfetas é o centro computacional dessas corretoras e bolsas, é a estrutura responsável por armazenar, organizar, atualizar e consultar instantaneamente todas as ofertas ativas no mercado. Como frações de milissegundo definem o lucro ou o prejuízo de uma operação financeira, a escolha de como esses dados são organizados na memória do computador é crucial.

## Esse Projeto
Implementamos duas versões completas de um sistema simulado de Livro de Ofertas, uma para cada tipo de árvore, que são versões modificadas das disponíveis em [https://github.com/RussellABrown/avl-and-red-black-trees/]
O programa permite:
* **Cadastrar novas ordens** de compra ou venda associadas a ativos da B3 (como `PETR4`, `VALE3`, etc.).
* **Executar ou cancelar ordens**, registrando elas em um histórico cronológico que guarda tudo que ocorreu, o que é útil para auditorias.
* **Consultar dados** por código de identificação ou por ativo.
* **Executar baterias de testes de desempenho (*benchmarks*)** que simulam cenários reais de estresse com milhares de operações, medindo tempo de processamento, consumo de memória e quantidade de reestruturações (rotações) exigidas pelas árvores.
