/* Copyright 2022 Olimpiev Y. */
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <x86intrin.h>
#include <cstring>

volatile union ticks{
    unsigned long long t64;
    struct s32 { unsigned long th, tl; } t32;
} start, end;

static size_t ArrayAccess(size_t size, int*(*order)(size_t)) {
    int* arr = order(size);
    unsigned long long time = 0;
    start.t64 =__rdtsc();
    for (int k = 0, i = 0; i < size; i++) {
        k = arr[k];
    }
    end.t64 = __rdtsc();
    time = (end.t64 - start.t64);
    free(arr);
    return time / size;
}

static size_t ArrayTour(size_t size, int*(*order)(size_t), size_t iterAmount) {
    size_t time = 0;
    // Warm up cache.
    ArrayAccess(size, order);
    
    for (size_t i = 1; i < iterAmount; i++) {
        time += ArrayAccess(size, order);
    }
    return time / iterAmount;
}

static int* InitCircleLinkedFwdArray(size_t size) {
    int* ret = (int*)calloc(size, sizeof(ret[0]));

    for (int i = 0; i < size; i++) {
        ret[i] = (i + 1) % size;
    }
    return ret;
}

static int* InitCircleLinkedRvsArray(size_t size) {
    int* ret = (int*)calloc(size, sizeof(ret[0]));

    for (int i = 0; i < size; i++) {
        ret[i] = size - 1 - i - 1;
    }
    return ret;
}

static int* InitCircleLinkedRndArray(size_t size) {
    std::srand(std::time(0));

    int* ret = (int*)calloc(size, sizeof(ret[0]));

    std::vector<int> indexes;
    indexes.reserve(size);
    for (size_t i = 0; i < size; i++) {
        indexes.push_back(i);
    }
    for (size_t i = 0; i < size; i++) {
        size_t index = std::rand() % indexes.size(); 
        ret[i] = indexes[index];
        indexes.erase(indexes.begin() + index);
    }
    return ret;
}

int main(int argc, char* argv[]) {
    size_t minSize = 128;
    size_t maxSize = 1000000;
    size_t currSize = minSize;
    // Default order = fwd.
    int*(*order)(size_t) = InitCircleLinkedFwdArray;
    
    if (argc == 2) {
        if (strcmp(argv[1], "fwd") == 0) {
            std::cout << "forward order on" << std::endl;
            order = InitCircleLinkedFwdArray;
        }
        if (strcmp(argv[1], "rvs") == 0) {
            std::cout << "reverse order on" << std::endl;
            order = InitCircleLinkedRvsArray;
        }
        if (strcmp(argv[1], "rnd") == 0) {
            std::cout << "random order on" << std::endl;
            order = InitCircleLinkedRndArray;
        }
    }
    while (currSize < maxSize) {
        size_t iterationsAmount = 3;
        size_t time = 0;
        time = ArrayTour(currSize, order, iterationsAmount);
        printf("Array size:%lu bytes.\tAverage access to an element:%lu ticks.\n", currSize * sizeof(int), time);
        currSize *= 2;
    }
    return EXIT_SUCCESS;
}

