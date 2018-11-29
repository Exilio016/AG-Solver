//
// Created by Bruno on 26/11/2018.
//

#ifndef OTIMIZACAOLINEAR_AG_MATRIXCUDA_H
#define OTIMIZACAOLINEAR_AG_MATRIXCUDA_H

int matrix_mult(float *result, float *m1, int rows_m1, int cols_m1, float *m2, int rows_m2, int cols_m2);

float *allocate_matrix(int rows, int cols);
float *transfer_matrix2device(float *m1, float *d_m1, int rows, int cols);
float *transfer_matrix2host(float *m1, float *d_m1, int rows, int cols);

#endif //OTIMIZACAOLINEAR_AG_MATRIXCUDA_H
