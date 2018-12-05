//
// Created by Bruno on 02/12/2018.
//

#include "utils.h"

#include <cstdlib>

void matrix_mult(float *result, const float *m1, int rows_m1, int cols_m1, const float *m2, int rows_m2, int cols_m2){
    for(int i = 0; i < rows_m1; i++){
        for(int j = 0; j < cols_m2; j++){
            result[i * cols_m2 + j] = 0;
            for(int k = 0; k < cols_m1; k++){
                result[i * cols_m2 + j] += m1[i * cols_m1 + k] * m2[i * cols_m2 + k];
            }
        }
    }

}

float randomFloat(int min, int max){

    return (((float ) (rand() % ((max - min + 1)*100)) / 100) + min);
}

int randomInt(int min, int max){
    return ((rand() % (max - min + 1)) + min);
}
