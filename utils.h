//
// Created by Bruno on 02/12/2018.
//

#ifndef OTIMIZACAOLINEAR_AG_UTILS_H
#define OTIMIZACAOLINEAR_AG_UTILS_H

void matrix_mult(float *result, const float *m1, int rows_m1, int cols_m1, const float *m2, int rows_m2, int cols_m2);
float randomFloat(int min, int max);
int randomInt(int min, int max);

#endif //OTIMIZACAOLINEAR_AG_UTILS_H
