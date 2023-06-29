/* Copyright 2022 Olimpiev Y. */
#include <cassert>
#include <cstdlib>
#include <gsl/gsl_blas.h>  
#include <iostream>
#include <limits>

static void PrintMatrix(const gsl_matrix_float* M) {
    assert(M);
    for (size_t i = 0; i < M->size1; i++) {
        for (size_t j = 0; j < M->size2; j++) {
            std::cout << gsl_matrix_float_get(M, i, j) << " ";
        }
        std::cout << std::endl;
    }
}

static gsl_matrix_float* MatrixMultiplication(const gsl_matrix_float* A, const gsl_matrix_float* B, gsl_matrix_float* res) {
    assert(A && B);
    assert(A->size2 == B->size1);

    for (size_t i = 0; i < A->size1; i++) {
        for (size_t k = 0; k < B->size2; k++) {
	        for (size_t j = 0; j < A->size2; j++ ) {
                res->data[i * res->size1 + k] += A->data[i * A->size1 + j] * B->data[j * B->size1 + k];
            }
        }
    }
    return res;
}

static gsl_matrix_float* MatrixSub(gsl_matrix_float* A, gsl_matrix_float* B, gsl_matrix_float* res) {
    assert(A && B);
    assert(A->size1 == B->size1 && A->size2 == B->size2);

    for (size_t row = 0; row < res->size1; row++) {
        for (size_t column = 0; column < res->size2; column++) {
            res->data[row * res->size1 + column] = A->data[row * res->size1 + column] - B->data[row * res->size1 + column];
        }
    }
    return res;
}

static gsl_matrix_float* MatrixSum(gsl_matrix_float* A, gsl_matrix_float* B, gsl_matrix_float* res) {
    assert(A && B);
    assert(A->size1 == B->size1 && A->size2 == B->size2);
    assert(res->size1 == A->size1 && res->size2 == A->size1);

    for (size_t row = 0; row < res->size1; row++) {
        for (size_t column = 0; column < res->size2; column++) {
            res->data[row * res->size1 + column] = A->data[row * res->size1 + column] + B->data[row * res->size1 + column];
        }
    }
    return res;
}

static void ScalarMultiplication(gsl_matrix_float* A, float scalar) {
    assert(A);
    for (size_t row = 0; row < A->size1; row++) {
        for (size_t column = 0; column < A->size2; column++) {
            A->data[row * A->size1 + column] *= scalar;
        }
    }
}

static void TransposeMatrix(gsl_matrix_float* A) {
    assert(A);
    assert(A->size1 == A->size2);

    for (size_t row = 0; row < A->size1; row++) {
        for (size_t column = row; column < A->size2; column++) {
            std::swap(A->data[row * A->size1 + column], A->data[column * A->size2 + row]);
        }
    }
}

static float MatrixNorm1(const gsl_matrix_float* A) {
    assert(A);
    float norm = std::numeric_limits<float>::min();
    for (size_t column = 0; column < A->size2; column++) {
        float sum = 0.0f;
        for (size_t row = 0; row < A->size1; row++) {
            sum += A->data[row * A->size1 + column];
        }
        norm = std::max(sum, norm);
    }
    return norm;
}

static float MatrixNormInf(const gsl_matrix_float* A) {
    assert(A);
    float norm = std::numeric_limits<float>::min();

    for (size_t row = 0; row < A->size1; row++) {
        float sum = 0.0f;
        for (size_t column = 0; column < A->size2; column++) {
            sum += A->data[row * A->size1 + column];
        }
        norm = std::max(norm, sum);
    }
    return norm;
}

static gsl_matrix_float* InverseCustom(const gsl_matrix_float* A, size_t seriesMemberAmount) {
    assert(A);
    assert(A->size1 == A->size2);

    gsl_matrix_float* I = gsl_matrix_float_calloc(A->size1, A->size2);
    gsl_matrix_float_set_identity(I);
   
    gsl_matrix_float* B = gsl_matrix_float_calloc(A->size1, A->size2);
    gsl_matrix_float_memcpy(B, A);
    TransposeMatrix(B);
    ScalarMultiplication(B, 1.0f / (MatrixNorm1(A) * MatrixNormInf(A)));
    
    gsl_matrix_float* res = gsl_matrix_float_calloc(A->size1, A->size2);
    
    gsl_matrix_float* tmp = gsl_matrix_float_calloc(A->size1, A->size2);
    MatrixMultiplication(B, A, tmp);

    gsl_matrix_float* R = gsl_matrix_float_calloc(A->size1, A->size2);
    MatrixSub(I, tmp, R);

    gsl_matrix_float* seriesMember = gsl_matrix_float_calloc(A->size1, A->size2);
    gsl_matrix_float_set_identity(seriesMember);

    for (size_t i = 0; i < seriesMemberAmount; i++) {
        gsl_matrix_float_set_zero(tmp);
        // res += R^i. seriesMember + res -> res.
        MatrixSum(res, seriesMember, res);
       
        // Calc R^i. seriesMember * R -> tmp.
        MatrixMultiplication(seriesMember, R, tmp);
        // Swap tmp and curr series member.
        std::swap(tmp, seriesMember);
    }

    MatrixMultiplication(res, B, tmp);

    gsl_matrix_float_free(I);
    gsl_matrix_float_free(R);
    gsl_matrix_float_free(B);
    gsl_matrix_float_free(res);
    gsl_matrix_float_free(seriesMember);
    return tmp;
}

int main(int argc, char* argv[]) {
    size_t matrixSize = 512;
    gsl_matrix_float* m = gsl_matrix_float_alloc(matrixSize, matrixSize);
    assert(m);
    gsl_matrix_float_set_identity(m);
    //gsl_matrix_float_set(m, 0, 0, 1);
    //gsl_matrix_float_set(m, 0, 1, 2);
    //gsl_matrix_float_set(m, 1, 0, 3);
    //gsl_matrix_float_set(m, 1, 1, 4);
    //PrintMatrix(m);
    
    // TEST Matrix Mult:
    /*
    gsl_matrix_float* n = gsl_matrix_float_calloc(matrixSize, matrixSize);
    assert(n);
    gsl_matrix_float_set(n, 0, 0, 1);
    gsl_matrix_float_set(n, 0, 1, 2);
    gsl_matrix_float_set(n, 1, 0, 3);
    gsl_matrix_float_set(n, 1, 1, 4);

    std::cout << "Mult res is:" << std::endl;
    gsl_matrix_float* resMult = gsl_matrix_float_calloc(matrixSize, matrixSize);
    assert(resMult);

    MatrixMultiplication(m, n, resMult);
    PrintMatrix(resMult);

    gsl_matrix_float_free(resMult);
    gsl_matrix_float_free(n);
    */

    auto res = InverseCustom(m, 10);
    PrintMatrix(res);

    gsl_matrix_float_free(res);
    gsl_matrix_float_free(m);
    
    return EXIT_SUCCESS;
}

