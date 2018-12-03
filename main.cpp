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

    problem->resolve(1000, 25);
    return 0;
}