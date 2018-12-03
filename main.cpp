#include <iostream>
#include "Linear.h"

int main() {
    Linear *problem;
    int nX, n;
    std::string func;

    std::cin >> nX;
    problem = new Linear(nX);
    std::cin >> n;

    int c = getc(stdin);
    if(c != '\n') ungetc(c, stdin);

    for(int i = 0; i < n; i++){
        std::getline(std::cin, func);
        problem->setConstraint(func);
    }

    std::getline(std::cin, func);
    problem->setZFunc(func);

    std::getline(std::cin, func);
    problem->setIntVars(func);

    std::getline(std::cin, func);
    problem->setBinVars(func);

    problem->resolve(1000, 25);
    return 0;
}