/* Copyright 2022 Olimpiev Y. */
#include <cassert>
#include <cstdlib>
#include <gsl/gsl_blas.h>  
#include <iostream>
#include <limits>

typedef float v4f __attribute__ ((vector_size (16)));
const size_t vecLen = 4;


static void PrintMatrix(const gsl_matrix_float* M) {
    assert(M);
    assert(M->size1 % vecLen == 0);
    for (size_t i = 0; i < M->size1; i++) {
        for (size_t j = 0; j < M->size2; j++) {
            std::cout << gsl_matrix_float_get(M, i, j) << " ";
        }
        std::cout << std::endl;
    }
}

static void TransposeMatrix(gsl_matrix_float* A) {
    assert(A);
    assert(A->size1 == A->size2);
    assert(A->size1 % vecLen == 0);

    for (size_t row = 0; row < A->size1; row++) {
        for (size_t column = row; column < A->size2; column++) {
            std::swap(A->data[row * A->size1 + column], A->data[column * A->size2 + row]);
        }
    }
}

static float dot(float* a, float* b, size_t n) {
    assert(a);
    assert(b);

    v4f res = {0.0f, 0.0f, 0.0f, 0.0f};
    v4f* aa = (v4f*)a;
    v4f* bb = (v4f*)b;

    for (size_t i = 0; i < n; i++) {
        res += aa[i] * bb[i];
    }
    return res[0] + res[1] + res[2] + res[3];
}

static gsl_matrix_float* MatrixMultiplication(const gsl_matrix_float* A, gsl_matrix_float* B, gsl_matrix_float* res) {
    assert(A && B);
    assert(A->size2 == B->size1);
    assert(A->size1 % vecLen == 0);

    v4f* resVec = (v4f*)res->data;
    v4f* bVec = (v4f*)B->data;
    TransposeMatrix(B);

    for (size_t i = 0; i < A->size1 ; i++) {
        for (size_t j = 0; j < A->size2 ; j++ ) {
            res->data[i * res->size1 + j] = dot(&A->data[i * A->size1], &B->data[j * B->size1], A->size2 / 4);
        }
    }
    TransposeMatrix(B);
    return res;
}

static gsl_matrix_float* MatrixSub(const gsl_matrix_float* A, const gsl_matrix_float* B, gsl_matrix_float* res) {
    assert(A && B);
    assert(A->size1 == B->size1 && A->size2 == B->size2);
    assert(A->size1 % vecLen == 0);

    v4f* aVec = (v4f*)A->data; 
    v4f* bVec = (v4f*)B->data;
    v4f* resVec = (v4f*)res->data;
    
    size_t size = A->size1 * A->size2;
    for (size_t i = 0; i < size / vecLen; i++) {
        resVec[i] = aVec[i] - bVec[i];
    }
    return res;
}

static gsl_matrix_float* MatrixSum(const gsl_matrix_float* A, const gsl_matrix_float* B, gsl_matrix_float* res) {
    assert(A && B);
    assert(A->size1 == B->size1 && A->size2 == B->size2);
    assert(A->size1 % vecLen == 0);
    v4f* aVec = (v4f*)A->data; 
    v4f* bVec = (v4f*)B->data;
    v4f* resVec = (v4f*)res->data;
    
    size_t size = A->size1 * A->size2;
    
    for (size_t i = 0; i < size / vecLen; i++) {
        resVec[i] = aVec[i] + bVec[i];
    }
    return res;
}

static void ScalarMultiplication(gsl_matrix_float* A, float scalar) {
    assert(A);
    assert(A->size1 % vecLen == 0);
    v4f* aVec = (v4f*)A->data;
    size_t size = A->size1 * A->size2;
    for (size_t i = 0; i < size / vecLen; i++) {
        aVec[i] *= scalar;
    }
}



static float MatrixNorm1(const gsl_matrix_float* A) {
    assert(A);
    assert(A->size1 % vecLen == 0);
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
    assert(A->size1 % vecLen == 0);
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

static void InverseCustom(gsl_matrix_float* A, gsl_matrix_float* res, size_t seriesMemberAmount) {
    assert(A);
    assert(res);
    assert(A->size1 == A->size2);
    assert(res->size1 == A->size1 && res->size2 == A->size2);
    assert(A->size1 % vecLen == 0);
    // Set I matrix for calc.
    gsl_matrix_float* I = gsl_matrix_float_calloc(A->size1, A->size2);
    gsl_matrix_float_set_identity(I);
   
    // Calc B = A^T / (norm1(A) * normInf(A)).
    gsl_matrix_float* B = gsl_matrix_float_calloc(A->size1, A->size2);
    gsl_matrix_float_memcpy(B, A);
    TransposeMatrix(B);
    ScalarMultiplication(B, 1.0f / (MatrixNorm1(A) * MatrixNormInf(A)));
    
    // Reserve tmp matrix for buffering.
    gsl_matrix_float* tmp = gsl_matrix_float_calloc(A->size1, A->size2);
    // tmp = B*A.
    MatrixMultiplication(B, A, tmp);
    
    // Calc R = I - B*A.
    gsl_matrix_float* R = gsl_matrix_float_calloc(A->size1, A->size2);
    MatrixSub(I, tmp, R);
    
    // Reserve memory for 
    gsl_matrix_float* seriesMember = gsl_matrix_float_calloc(A->size1, A->size2);
    gsl_matrix_float_set_identity(seriesMember);

    for (size_t i = 0; i < seriesMemberAmount; i++) {
        gsl_matrix_float_set_zero(tmp);
        // res += R^i. seriesMember + res -> res.
        MatrixSum(seriesMember, res, res);
        // Calc R^i. seriesMember * R -> tmp.
        MatrixMultiplication(seriesMember, R, tmp);
        std::swap(seriesMember, tmp);
    }
    // Calc (I + R + R^2 + ...) * B.
    gsl_matrix_float_set_zero(tmp);
    MatrixMultiplication(res, B, tmp);
    
    gsl_matrix_float_memcpy(res, tmp);

    gsl_matrix_float_free(I);
    gsl_matrix_float_free(R);
    gsl_matrix_float_free(B);
    gsl_matrix_float_free(tmp);
    gsl_matrix_float_free(seriesMember);
}

int main(int argc, char* argv[]) {
    size_t matrixSize = 512;
    gsl_matrix_float* m = gsl_matrix_float_alloc(matrixSize, matrixSize);
    assert(m);

    gsl_matrix_float_set_identity(m);
    /*
    gsl_matrix_float_set(m, 0, 0, 1);
    gsl_matrix_float_set(m, 0, 1, 2);
    gsl_matrix_float_set(m, 0, 2, -1);
    gsl_matrix_float_set(m, 0, 3, 2);
    gsl_matrix_float_set(m, 1, 0, 0);
    gsl_matrix_float_set(m, 1, 1, 3);
    gsl_matrix_float_set(m, 1, 2, 1);
    gsl_matrix_float_set(m, 1, 3, 1);
    gsl_matrix_float_set(m, 2, 0, 2);
    gsl_matrix_float_set(m, 2, 1, 2);
    gsl_matrix_float_set(m, 2, 2, 3);
    gsl_matrix_float_set(m, 2, 3, -1);
    gsl_matrix_float_set(m, 3, 0, 0);
    gsl_matrix_float_set(m, 3, 1, 2);
    gsl_matrix_float_set(m, 3, 2, -1);
    gsl_matrix_float_set(m, 3, 3, 1);

    PrintMatrix(m);
    */
    gsl_matrix_float* res = gsl_matrix_float_calloc(matrixSize, matrixSize);
    assert(res);

    InverseCustom(m, res, 10);
    PrintMatrix(res);

    gsl_matrix_float_free(m);
    gsl_matrix_float_free(res);

    return EXIT_SUCCESS;
}

