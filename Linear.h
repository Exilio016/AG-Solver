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

    int num_x, rows_a, cols_a, num_gaps;
    bool isMin;

    int mountFunc(std::string exp);
    int mountRestriction(std::string exp, int row);
    void mountIntVars(std::string exp);
    void mountBinVars(std::string exp);
    void genPop(int numPop);
    int evaluate(float *d_ct, float *d_x, float *d_res, float *ret);
    void newPop(int best_index);
    int isFactible(int index);
    void preprocess();
    int prepare();
public:
    explicit Linear(int n);
    void setZFunc(std::string exp);
    void setRestriction(std::string exp);
    int resolve(int numGens, int numPop, float *z);
    void setIntVars(std::string exp);
    void setBinVars(std::string exp);

    float *getX(int index);
};


#endif //OTIMIZACAOLINEAR_AG_LINEAR_H
