#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <x86intrin.h>

constexpr size_t maxFragmentsAmount = 64;
constexpr size_t Kb = 1024;
constexpr size_t l1Size = 32 * Kb;
constexpr size_t l2Size = 512 * Kb;

int main(void) {
    size_t cacheSize =  2 * l2Size;
    int* a = new int[cacheSize * maxFragmentsAmount]; 
    //std::fstream out("res.csv", std::ios::out | std::ios::app);
    //assert(out.is_open());
    //for (size_t iters = 0; iters < 100; iters++) {
        for (int fragmentsAmount = 1; fragmentsAmount <= maxFragmentsAmount; fragmentsAmount++) {
            size_t currSize = cacheSize * fragmentsAmount;
            for (size_t i = 0; i < currSize; i++) {
                a[i] = (1 + i + cacheSize) % currSize;
            }
            unsigned long long start = __rdtsc();
            for(int k = 0, i = 0; i < currSize; i++) {
                k = a[k];
            }
            unsigned long long end = __rdtsc();
            //out << (end - start) / currSize << ", "; 
            std::cout << "fragments: " << fragmentsAmount << " " << "\ttics: " << (end - start) / currSize << std::endl;
        }
    //    out << std::endl;
    //    std::cout << "Iter ends." << std::endl;
    //}
    delete a;
    //out.close();
    return EXIT_SUCCESS;
}

