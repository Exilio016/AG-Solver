#include <iostream>
#include "Linear.h"

int main() {
    Linear *problem;
    int nX, n;
    std::string func;

    std::cout << "Digite o número de variáveis do problema: ";
    std::cin >> nX;
    problem = new Linear(nX);

    std::cout << "Digite o número de restrições do problema: ";
    std::cin >> n;

    int c = getc(stdin);
    if(c != '\n') ungetc(c, stdin);

    std::cout << "Digite as restrições do problema, uma por linha:\n";
    for(int i = 0; i < n; i++){
        std::getline(std::cin, func);
        problem->setConstraint(func);
    }

    std::cout <<"Digite a fução objetivo: ";
    std::getline(std::cin, func);
    problem->setZFunc(func);

    std::cout <<"Digite quais variáveis são inteiras (separadas por vírigula), ou ENTER se não houver:\n";
    std::getline(std::cin, func);
    problem->setIntVars(func);

    std::cout <<"Digite quais variáveis são binárias (separadas por vírigula), ou ENTER se não houver:\n";
    std::getline(std::cin, func);
    problem->setBinVars(func);

    int i = problem->resolve(1000, 25);

    if(i < 0){
        std::cout << "O AG não encontrou solução factível para o problema!\n";
    }
    else{
        std::cout << "A melhor solução encontrada em 1000 gerações foi Z = " << std::to_string(problem->getZ(i)) <<"\n";
        std::cout << "Os valores de X dessa solução é:\n";
        float *x = problem->getX(i);

        for(int j = 0; j < nX; j++){
            std::cout << std::to_string(x[j]) << " ";
        }
        std::cout << "\n";

    }


    return 0;
}
