#include <utility>

#include <iostream>
#include <limits>
#include <cmath>
#include <ctime>

#include "Linear.h"
#include "utils.h"

#define LESS_AND_EQUAL 1
#define MORE_AND_EQUAL 0
#define EQUAL 2

float global_max = 0;


struct VAR_RULE{
    int isLess;
    int min;
    int max;
    int isInt;
    int isBin;
};

struct POPULATION {
    float z;
    float *x;
};

using namespace std;

Linear::Linear(int n) {
    this->num_x = n;
    this->cols_a = n;
    this->rows_a = 0;
    this->num_surplus = 0;
}

int Linear::mountFunc(string exp) {
    if(exp.compare(0, 3, "min") == 0)
        this->isMin = true;
    else if(exp.compare(0, 3, "max") == 0)
        this->isMin = false;
    else
        return 0;

    string aux;
    float aux_value;
    int pos;
    bool isPos = false;
    bool preencheu = true;

    for(int i = 3; i < exp.length(); i++){
        char c = exp[i];

        if ((c == '+' || c == '-')){
            pos = atoi(aux.c_str());
            this->ct[pos] = aux_value;

            aux.clear();
            preencheu = true;
            isPos = false;
        }

        if(c == '-' || (c == '.' && !isPos) || (c >= '0' && c <='9')) {
            aux.append(1, c);
            preencheu = false;
        }
        if(c == 'x'){
            aux_value = (float) atof(aux.c_str());
            aux.clear();
            preencheu = true;
            isPos = true;

            if(aux_value > global_max) global_max = aux_value;
        }
    }
    if(!preencheu){
        pos = atoi(aux.c_str());
        this->ct[pos] = aux_value;
    }

    return 1;
}

int Linear::mountRestriction(std::string exp, int row) {
    int pos;
    bool preencheu = false, isPos = false;
    float aux_value;
    string aux;

    for(int i = 0; i < exp.length(); i++){
        char c = exp[i];

        if ((c == '+' || c == '-' || c == '<' || c == '>' || c == '=')){
            pos = atoi(aux.c_str());
            this->a[row * cols_a + pos] = aux_value;
            aux.clear();
            preencheu = true;
            isPos = false;
        }

        if(c == '-' || (c == '.' && !isPos) || (c >= '0' && c <='9')) {
            aux.append(1, c);
            preencheu = false;
        }
        if(c == 'x'){
            aux_value = (float) atof(aux.c_str());
            aux.clear();
            preencheu = true;
            isPos = true;

            if(aux_value > global_max) global_max = aux_value;
        }

        if(c == '>'){
            if(exp[i+1] == '=') i++;
            this->a[row * cols_a + num_x + row] = -1;

            var_rule_t rule;
            rule.min = 0;
            rule.isLess = MORE_AND_EQUAL;

            this->rules[row+num_x] = rule;
        }
        if(c == '<'){
            if(exp[i+1] == '=') i++;
            this->a[row * cols_a + num_x + row] = 1;

            var_rule_t rule;
            rule.min = 0;
            rule.isLess = LESS_AND_EQUAL;

            this->rules[row+num_x] = rule;
        }
        if(c == '='){
            var_rule_t rule;
            rule.min = 0;
            rule.isLess = EQUAL;

            this->rules[row+num_x] = rule;
        }
    }

    if(!preencheu){
        aux_value = (float) atof(aux.c_str());
        b[row] = aux_value;

        if(aux_value > global_max) global_max = aux_value;
    }

    return 1;
}

void Linear::setZFunc(std::string exp) {
    this->func = std::move(exp);
}

void Linear::setConstraint(std::string exp) {
    this->restrictions.push_back(exp);
    this->num_surplus++;
    this->cols_a++;
    this->rows_a++;
}

float *matrix_create(int rows, int cols){
    float *matrix = new float[rows * cols];

    for(int i = 0; i < rows*cols; i++) matrix[i] = 0;

    return matrix;
}

void print_matrix(float *m,  int rows, int cols){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++ ){
            std::cout << to_string(m[i*cols + j]) << ' ';
        }
        std::cout << '\n';
    }

}

int Linear::prepare() {
    this->ct = matrix_create(1, num_x + num_surplus);
    this->a = matrix_create(rows_a, cols_a);
    this->b = matrix_create(rows_a, 1);
    this->rules = (var_rule_t *) (malloc(sizeof(var_rule_t) * (num_x + num_surplus)));

    for(int i = 0; i < rows_a; i++){
        string exp = restrictions[i];
        if(this->mountRestriction(exp, i) == 0)
            return 0;
    }

    for(int i = 0; i < num_x; i++){
        rules[i].min = 0;
        rules[i].max = (int) global_max;
        rules[i].isInt = 0;
    }

    mountIntVars(intRes);
    mountBinVars(binRes);

    return this->mountFunc(func);
}

void Linear::preProcess() {

    for(int i = 0; i < num_x; i++){
        for(int j = 0; j < rows_a; j++){
            if(rules[num_surplus + j].isLess == LESS_AND_EQUAL && a[j * cols_a + i] > 0){
                int aux = (int) (b[j] / a[j * cols_a + i]);
                if(aux < rules[i].max) rules[i].max = aux;
            }
            else if(rules[num_surplus + j].isLess == MORE_AND_EQUAL && a[j * cols_a + i] < 0){
                int aux = (int) (b[j] / -1*a[j * cols_a + i]);
                if(aux < rules[i].max) rules[i].max = aux;
            }
        }
    }

}

void generateGaps(float *x, float *b, float *a, int num_x, int num_gaps, int cols_a, var_rule_t *rules){
    for(int i = num_x; i < (num_x + num_gaps); i++){
        int row = i - num_x;
        if(rules[i].isLess == EQUAL){
            x[i] = 0;
        }

        else if(rules[i].isLess == LESS_AND_EQUAL){
            x[i] = b[row];
            for(int j = 0; j < num_x; j++){
                x[i] -= a[row*cols_a + j] * x[j];
            }
        }

        else if(rules[i].isLess == MORE_AND_EQUAL){
            x[i] = 0;
            for(int j = 0; j < num_x; j++){
                x[i] += a[row*cols_a + j] * x[j];
            }
            x[i] -= b[i - num_x];
        }
    }
}


void genX(float *x, float *b, float *a, int num_x, int num_gaps, int cols_a, var_rule_t *rules){
    for(int i = 0; i < num_x; i++){
        if(rules[i].isInt == 1)
            x[i] = randomInt(rules[i].min, rules[i].max);
        else
           x[i] = randomFloat(rules[i].min, rules[i].max);
    }

    generateGaps(x, b, a, num_x, num_gaps, cols_a, rules);

}

void Linear::genPop(int numPop) {
    pop = new population_t *[numPop];
    if(pop != NULL){
        this->num_pop = numPop;
        for(int i = 0; i < numPop; i++){
            pop[i] = new population_t;
            pop[i]->x = matrix_create(num_x + num_surplus, 1);

            genX(pop[i]->x, b, a, num_x, num_surplus, cols_a, rules);
        }
    }
}
#define T_MUT 10
void cruzar(float *x1, float *x2, float *a, float *b, var_rule_t *rules, int num_x, int num_gaps, int cols_a){

    for(int i = 0; i < num_x; i++){
        if(rules[i].isBin == 1){
            if(randomInt(0, 100) <= 50)
                x1[i] = x2[i];
        }
        else if(rules[i].isInt == 1)
            x1[i] = ceil((x1[i] + x2[i])/2);
        else
            x1[i] = (x1[i] + x2[i])/2;

        if(randomInt(0, 100) <= T_MUT) {
            if(rules[i].isBin == 1){
                if(x1[i] == 0) x1[i] = 1;
                else x1[i] = 0;
            }
            else if(rules[i].isInt == 1)
                x1[i] += randomInt((int) -global_max, (int) global_max);
            else
                x1[i] += randomFloat((int) -global_max, (int) global_max);
        }
    }

    generateGaps(x1, b, a, num_x, num_gaps, cols_a, rules);
}

int Linear::resolve(int numGens, int numPop) {
    int best = -1;

    prepare();
    preProcess();

    srand(time(NULL));
    genPop(numPop);
    for(int i = 0; i < numGens; i++){
        best = evaluate();
        if(best >= 0){
            newPop(best);
        }
        else break;

        //Print x
        for(int i = 0; i < num_x; i++) {
            std::cout << to_string(pop[best]->x[i]);
            if(i < num_x - 1)
                std::cout << ", ";
        }
        std::cout << "\n";
    }

    return best;
}

int Linear::evaluate() {
    float best, res;
    int pos_best = -1;
    bool first = true;
    for(int i = 0; i < num_pop; i++){
        if(isFeasible(i)) {
            matrix_mult(&res, ct, 1, num_x + num_surplus, pop[i]->x, num_x + num_surplus, 1);
            pop[i]->z = res;

            if (isMin) {
                if (first || res < best) {
                    pos_best = i;
                    best = res;
                    first = false;
                }
            } else {
                if (first || res > best) {
                    pos_best = i;
                    best = res;
                    first = false;
                }
            }
        }
    }
    return pos_best;
}

void Linear::newPop(int best_index) {
    for(int i = 0; i < num_pop; i++){
        if(i != best_index)
            cruzar(pop[i]->x, pop[best_index]->x, a, b, rules, num_x, num_surplus, cols_a);
    }
}

int Linear::isFeasible(int index) {
    float *x = pop[index]->x;
    for(int i = 0; i < (num_surplus + num_x); i++){
        if(x[i] < 0) return 0;
    }
    return 1;

}

float *Linear::getX(int index) {
    return pop[index]->x;
}

void Linear::mountIntVars(string exp) {
    string aux;
    int pos;
    for(char c: exp){
        if(c >= '0' && c <= '9'){
            aux.append(1, c);
        }

        if(c == ','){
            pos = atoi(aux.c_str());
            aux.clear();
            rules[pos].isInt = 1;
        }
    }
    if(aux.length() > 0){
        pos = atoi(aux.c_str());
        aux.clear();
        rules[pos].isInt = 1;
    }
}

void Linear::mountBinVars(string exp) {
    string aux;
    int pos;
    for(char c: exp){
        if(c >= '0' && c <= '9'){
            aux.append(1, c);
        }

        if(c == ','){
            pos = atoi(aux.c_str());
            aux.clear();
            rules[pos].isInt = 1;
            rules[pos].isBin = 1;
            rules[pos].min = 0;
            rules[pos].max = 1;
        }
    }
    if(aux.length() > 0){
        pos = atoi(aux.c_str());
        aux.clear();
        rules[pos].isInt = 1;
        rules[pos].isBin = 1;
        rules[pos].min = 0;
        rules[pos].max = 1;
    }
}

void Linear::setIntVars(std::string exp) {
    this->intRes = std::move(exp);
}

void Linear::setBinVars(std::string exp) {
    this->binRes = std::move(exp);
}

float Linear::getZ(int index) {
    return this->pop[index]->z;
}

