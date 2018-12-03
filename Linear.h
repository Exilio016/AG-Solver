//
// Created by Bruno on 26/11/2018.
//

#ifndef OTIMIZACAOLINEAR_AG_LINEAR_H
#define OTIMIZACAOLINEAR_AG_LINEAR_H

#include <string>
#include <vector>


typedef struct VAR_RULE var_rule_t;
typedef struct POPULATION population_t;

class Linear {
private:
    float *ct , *a,  *b;
    population_t **pop;
    var_rule_t *rules;
    std::vector<std::string> restrictions;
    std::string func;
    std::string intRes, binRes;
    int num_pop;

    int num_x, rows_a, cols_a, num_surplus;
    bool isMin;

    int mountFunc(std::string exp);
    int mountRestriction(std::string exp, int row);
    void mountIntVars(std::string exp);
    void mountBinVars(std::string exp);
    void genPop(int numPop);
    int evaluate();
    void newPop(int best_index);
    int isFeasible(int index);
    void preProcess();
    int prepare();

public:
    /**
     * Construtor da classe Linear que irá resolver o problema de otimização linear
     * @param n - Número de váriaveis do problema de otimização a ser resolvido
     */
    explicit Linear(int n);

    /**
     * Método que adiciona a funcão de otimização no formato:
     * 'min a0x0 + a1x1 + ... + anxn' ou 'max a0x0 + a1x1 + ... + anxn'
     * com ai sendo um número real, com i = 0,1,2,...,n
     * @param exp - função de otimização
     */
    void setZFunc(std::string exp);

    /**
     * Método que adiciona uma restrição ao problema de otmização, no formato:
     * 'a0x0 + ... + anxn <= b' ou 'a0x0 + ... + anxn >= b' ou 'a0x0 + ... + anxn = b'
     *  com ai sendo um número real, com i = 0,1,2,...,n
     * @param exp - restrição a ser adicionada
     */
    void setConstraint(std::string exp);

    /**
     * Método que adiciona uma restrição de variáveis inteiras no formato
     * 'xa, xb, xc, ...', com a, b e c sendo números inteiros referentes aos indices,
     * que deseja que a variável seja inteira
     * @param exp - restrição a ser adicionada
     */
    void setIntVars(std::string exp);

    /**
     * Método que adiciona uma restrição de variáveis binárias no formato
     * 'xa, xb, xc, ...', com a, b e c sendo números inteiros referentes aos indices,
     * que deseja que a variável seja binária
     * @param exp - restrição a ser adicionada
     */
    void setBinVars(std::string exp);

    /**
     * Método que resolve o problema de otimização utilizando um algorítimo evolutivo
     * Esse método só irá funcionar corretamente se a função de otimização, as restrições,
     * as variáveis inteiras e as binárias já tenham sido adicionadas ao problema
     * @param numGens - Número de gerações do algorítimo evolutivo
     * @param numPop - Número de indivíduos na população do algorítimo evolutivo
     * @return indíce do vetor de população que contém a melhor solução
     */
    int resolve(int numGens, int numPop);

    /**
     * Retorna o vetor x, ou seja o vetor contendo uma solução do problema de otimização,
     * na posição dada do vetor de população, esse método só irá funcionar corretamente,
     * se executado após o método resolve.
     * @param index - indíce do vetor de população que contém a solução desejada
     * @return vetor x contendo a solução para o problema de otimização
     */
    float *getX(int index);

    /**
     * Retorna o valor Z, ou seja o resultado da função de otimização,
     * na posição dada do vetor de população, esse método só irá funcionar corretamente,
     * se executado após o método resolve.
     * @param index - indíce do vetor de população que contém a solução desejada
     * @return valor Z resultado da função de otimização
     */
    float getZ(int index);
};


#endif //OTIMIZACAOLINEAR_AG_LINEAR_H
