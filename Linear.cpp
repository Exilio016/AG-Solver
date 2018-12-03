#include <utility>

#include <iostream>
#include <limits>
#include <cmath>
#include <ctime>

#include "Linear.h"
#include "utils.h"

#define LESS_AND_EQUAL 1
#define GREATER_AND_EQUAL 0
#define EQUAL 2
#define T_MUT 10

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

    //Loop que le uma string de restrição e monta a linha da matriz
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
            rule.isLess = GREATER_AND_EQUAL;

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
    this->ct = matrix_create(1, num_x + num_surplus); //Cria a matriz de custo (função objetivo)
    this->a = matrix_create(rows_a, cols_a); //Cria a matriz A (referente as restrições)
    this->b = matrix_create(rows_a, 1); //Cria a matriz B (referente as restrições)
    this->rules = (var_rule_t *) (malloc(sizeof(var_rule_t) * (num_x + num_surplus))); //Cria o vetor de regras (limite das variáveis)

    //Loop que preenche a matriz A de acordo com as restrições dadas
    for(int i = 0; i < rows_a; i++){
        string exp = restrictions[i];
        if(this->mountRestriction(exp, i) == 0)
            return 0;
    }

    //Loop que cria o limitante global das variáveis (gambiarra)
    for(int i = 0; i < num_x; i++){
        rules[i].min = 0;
        rules[i].max = (int) global_max;
        rules[i].isInt = 0;
    }

    //Configura as variáveis inteiras
    mountIntVars(intRes);


    //Configura as variáveis binárias
    mountBinVars(binRes);

    //Preenche a matriz de custo e retorna
    return this->mountFunc(func);
}

void Linear::preProcess() {

    for(int i = 0; i < num_x; i++){
        for(int j = 0; j < rows_a; j++){
            if(isMin) {
                if (rules[num_surplus + j].isLess == LESS_AND_EQUAL && a[j * cols_a + i] > 0) {
                    int aux = (int) (b[j] / a[j * cols_a + i]);
                    if (aux < rules[i].max) rules[i].max = aux;
                } else if (rules[num_surplus + j].isLess == GREATER_AND_EQUAL && a[j * cols_a + i] < 0) {
                    int aux = (int) (b[j] / (-1 * a[j * cols_a + i]));
                    if (aux < rules[i].max) rules[i].max = aux;
                }
            }

            else{
                if (rules[num_surplus + j].isLess == GREATER_AND_EQUAL && a[j * cols_a + i] > 0) {
                    int aux = (int) (b[j] / a[j * cols_a + 1]);
                    if (aux > rules[i].min) rules[i].min = aux;
                }
                else if(rules[num_surplus + j].isLess == LESS_AND_EQUAL && a[j * cols_a + i] < 0) {
                    int aux = (int) (b[j] / (-1* a[j * cols_a + i]));
                    if (aux > rules[i].min) rules[i].min = aux;
                }
            }
        }
    }

}

void generateSurplus(float *x, float *b, float *a, int num_x, int num_surplus, int cols_a, var_rule_t *rules){

    //Para cada variável de folga, é calculado seu valor de acordo com a sua restrição correspondente
    for(int i = num_x; i < (num_x + num_surplus); i++){
        int row = i - num_x; //Linha da matriz a referente a restrição da variável atual

        //Se a restrição for de igual, não existe variável de folga, ou seja valor 0
        if(rules[i].isLess == EQUAL){
            x[i] = 0;
        }

        //Se a restrição for f(x0,..,xn) <= b, a variável é igual a b - f(x0,...,xn)
        else if(rules[i].isLess == LESS_AND_EQUAL){
            x[i] = b[row];
            for(int j = 0; j < num_x; j++){
                x[i] -= a[row*cols_a + j] * x[j];
            }
        }

        //Se a restrição for f(x0,..,xn) >= b, a variável é igual a f(x0,...,xn) - b
        else if(rules[i].isLess == GREATER_AND_EQUAL){
            x[i] = 0;
            for(int j = 0; j < num_x; j++){
                x[i] += a[row*cols_a + j] * x[j];
            }
            x[i] -= b[row];
        }
    }
}


void genX(float *x, float *b, float *a, int num_x, int num_gaps, int cols_a, var_rule_t *rules){

    //Cada variável do problema é inicializada com valor aleatório
    for(int i = 0; i < num_x; i++){
        if(rules[i].isInt == 1)
            //Gera numero aleatório inteiro
            x[i] = randomInt(rules[i].min, rules[i].max);
        else
            //Gera número aleatório real
            x[i] = randomFloat(rules[i].min, rules[i].max);
    }

    //Gera as variáveis de folga, usada para verificar as restrições do problema
    generateSurplus(x, b, a, num_x, num_gaps, cols_a, rules);

}

void Linear::genPop(int numPop) {
    pop = new population_t *[numPop]; //Cria o vetor de população


    if(pop != NULL){
        this->num_pop = numPop;
        for(int i = 0; i < numPop; i++){
            pop[i] = new population_t;
            pop[i]->x = matrix_create(num_x + num_surplus, 1); //Cria a matriz X daquele indivíduo

            //Preenche a matriz X com valores aleatórios
            genX(pop[i]->x, b, a, num_x, num_surplus, cols_a, rules);
        }
    }
}
void cruzar(float *x1, float *x2, float *a, float *b, var_rule_t *rules, int num_x, int num_surplus, int cols_a){

    //Itera entre todas as variáveis do indivíduo
    for(int i = 0; i < num_x; i++){
        //Se a variável for binária x1[i] ou mantém seu valor, ou recebe o valor de x2[i]
        if(rules[i].isBin == 1){
            if(randomInt(0, 100) <= 50)
                x1[i] = x2[i];
        }

        //Se a variável for inteira x1[i] recebe o chão do média de x1[i] e x2[1]
        else if(rules[i].isInt == 1)
            x1[i] = ceil((x1[i] + x2[i])/2);

        //Se a variável for real, x1[i] recebe a média de x1[i] e x2[i]
        else
            x1[i] = (x1[i] + x2[i])/2;

        //Mutação da variável
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

    //Após cruzar as variáveis é preciso calcular as novas variáveis de folga
    generateSurplus(x1, b, a, num_x, num_surplus, cols_a, rules);
}

int Linear::resolve(int numGens, int numPop) {
    int best = -1;

    //Processa as strings e monta as matrizes do problema
    prepare();

    //Define os limitantes das variáveis, para gerar mais indivíduos na região factível
    preProcess();

    srand(time(NULL));
    //Gera a população com indivíduos aleatórios
    genPop(numPop);

    //Loop das gerações
    for(int i = 0; i < numGens; i++){
        //Encontra o melhor indivíduo da população
        best = evaluate();

        //Se best for < 0, não houve indivíduo factível
        if(best >= 0){
            //Gera uma nova população, cruzando com o melhor de todos
            newPop(best);
        }
        else break;

        //Imprime na tela os valores de x do melhor de todos da geração
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

    //Para cada indivíduo da população é calculado o valor da função objetivo, ou seja, ct * x
    for(int i = 0; i < num_pop; i++){
        //Se a variável for infactível, a respostá é inválida, portanto não é necessário calcular
        if(isFeasible(i)) {
            //res = ct * x
            matrix_mult(&res, ct, 1, num_x + num_surplus, pop[i]->x, num_x + num_surplus, 1); //multiplicação de matrizes
            pop[i]->z = res;

            //Verifica se a solução é melhor
            if (isMin) { //Se o problema for de minimizar

                if (first || res < best) {//Se a solução for melhor
                    pos_best = i;
                    best = res;
                    first = false;
                }

            } else { //Se o problema for de maximizar

                if (first || res > best) { //Se a solução for melhor
                    pos_best = i;
                    best = res;
                    first = false;
                }
            }
        }
    }
    //Retorna o indice do melhor de todos
    return pos_best;
}

void Linear::newPop(int best_index) {
    //Para cada individuo da população, menos o melhor, cruzar com o melhor de todos
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

