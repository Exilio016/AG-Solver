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
        problem->setRestriction(func);
    }

    std::getline(std::cin, func);
    problem->setZFunc(func);

    std::getline(std::cin, func);
    problem->setIntVars(func);

    std::getline(std::cin, func);
    problem->setBinVars(func);

    float z;
    int i = problem->resolve(1000, 25, &z);

    if(i >= 0) {
        float *x = problem->getX(i);

        for (int i = 0; i < nX; i++)
            std::cout << std::to_string(x[i]) << " ";
        std::cout << "\n";

    }
    return 0;
}