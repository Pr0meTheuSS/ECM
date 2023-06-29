#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <x86intrin.h>

int main(void) {
        char a[1024 * 1024][1024];
        for (size_t i = 0; i < 1024; i++) {
            int k = 0;
            unsigned long long start = __rdtsc();
            for (size_t j = 0; j < 1024 * 1024; j++) {
                    k = a[j][i];
            }
            unsigned long long end = __rdtsc();
        //out << (end - start) / currArray << std::endl;  
        std::cout << "\ttics: " << (end - start) / 1024 / 1024 << std::endl;
        }
    return EXIT_SUCCESS;
}
    //out.close();

