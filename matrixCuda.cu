//
// Created by Bruno on 26/11/2018.
//

#include "matrixCuda.h"
#include <stdlib.h>
#include <cuda.h>

__global__ void mult(float *res, float *m1, int rows_m1, int cols_m1, float *m2, int rows_m2, int cols_m2){
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    int tid = y * cols_m2 + x;

    if(tid < rows_m1 * cols_m2){
        res[tid] = 0;
        for(int i = 0; i < cols_m1; i++){
            res[tid] += m1[y*cols_m1 + i] * m2[i*cols_m2 + x];
        }
    }
}

float *transfer_matrix2device(float *m1, float *d_m1, int rows, int cols){
    cudaMemcpy(d_m1, m1, sizeof(float) * rows * cols, cudaMemcpyHostToDevice);
}
float *transfer_matrix2host(float *m1, float *d_m1, int rows, int cols){
    cudaMemcpy(m1, d_m1, sizeof(float) * rows * cols, cudaMemcpyDeviceToHost);
}

int matrix_mult(float *result, float *m1, int rows_m1, int cols_m1, float *m2, int rows_m2, int cols_m2){
    if(cols_m1 != rows_m2) return 0;

    dim3 blockDim(rows_m1 / 2 + rows_m1 % 2 > 0, cols_m2/2 + cols_m2 % 2 > 0);
    dim3 gridDim(rows_m1 / 2, cols_m2/2);

    mult<<<gridDim, blockDim>>>(result, m1, rows_m1, cols_m1, m2, rows_m2, cols_m2);

}

float *allocate_matrix(int rows, int cols){
    float *matrix;
    cudaMalloc(&matrix, sizeof(float) * rows * cols);

    return matrix;
}
