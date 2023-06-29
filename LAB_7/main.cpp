/* Copyright 2022 Olimpiev Y. */
#include <cassert>
#include <gsl/gsl_blas.h>  
#include <cstdlib>
#include <iostream>

static void PrintMatrix(const gsl_matrix_float* M) {
    for (size_t i = 0; i < M->size1; i++) {
        for (size_t j = 0; j < M->size2; j++) {
            std::cout << gsl_matrix_float_get(M, i, j) << " ";
        }
        std::cout << std::endl;
    }
}

void MatrixAdd(gsl_matrix_float* A, gsl_matrix_float* B) {
    assert(A->size1 == B->size1 && A->size2 == B->size2);
    for (size_t i = 0; i < A->size1; i++) {
        auto aRow = gsl_matrix_float_row(A, i);
        auto bRow = gsl_matrix_float_row(B, i);
        gsl_vector_float_add(&aRow.vector, &bRow.vector);
    }
}

gsl_matrix_float* InverseBlas(gsl_matrix_float* A, size_t seriesMemberAmount) {
    assert(A);
    assert(A->size1 == A->size2);

    gsl_matrix_float* I = gsl_matrix_float_calloc(A->size1, A->size2);
    gsl_matrix_float_set_identity(I);

    gsl_matrix_float* res = gsl_matrix_float_calloc(A->size1, A->size2);
    gsl_matrix_float_set_zero(res);

    float norm1 = gsl_matrix_float_norm1(A);
 
    gsl_matrix_float_transpose(A);
    float normInf = gsl_matrix_float_norm1(A);
    gsl_matrix_float_transpose(A);
    
    assert(norm1 != 0.0);
    assert(normInf != 0.0);

    gsl_matrix_float* R = gsl_matrix_float_calloc(A->size1, A->size2);
    gsl_matrix_float_set_identity(R);
    
    // Calculate R = I - BA.
    gsl_blas_sgemm(CblasTrans, CblasNoTrans, -1.0f / (norm1 * normInf), A, A, 1.0, R);
    
    gsl_matrix_float* currMember = gsl_matrix_float_calloc(A->size1, A->size2);
    gsl_matrix_float_set_identity(currMember);
    
    gsl_matrix_float* tmp = gsl_matrix_float_calloc(A->size1, A->size2);
    for (size_t i = 0; i < seriesMemberAmount; i++) {
        MatrixAdd(res, currMember);
        gsl_blas_sgemm(CblasNoTrans, CblasNoTrans, 1.0, R, currMember, 0.0, tmp);
        gsl_matrix_float_memcpy(currMember, tmp);
    }
    
    gsl_blas_sgemm(CblasNoTrans, CblasTrans, 1.0f / (norm1 * normInf), res, A, 0.0, tmp);
    
    gsl_matrix_float_free(I);
    gsl_matrix_float_free(R);
    gsl_matrix_float_free(currMember);
    gsl_matrix_float_free(res);

    return tmp;
}


int main(int argc, char* argv[]) {
    size_t matrixSize = 512;
    gsl_matrix_float* m = gsl_matrix_float_alloc(matrixSize, matrixSize);
    assert(m);
    gsl_matrix_float_set_identity(m);
    /*
    gsl_matrix_float_set(m, 0, 0, 1);
    gsl_matrix_float_set(m, 0, 1, 2);
    gsl_matrix_float_set(m, 1, 0, 3);
    gsl_matrix_float_set(m, 1, 1, 4);

    PrintMatrix(m);
    */
    auto res = InverseBlas(m, 10);
    PrintMatrix(res);
    
    gsl_matrix_float_free(m);
    gsl_matrix_float_free(res);
    return EXIT_SUCCESS;
}

