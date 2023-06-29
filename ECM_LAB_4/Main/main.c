#include <stdlib.h>
#include <stdio.h>
#include "CalcLn.h"

int main(void) {
    const size_t seriesMembersAmount = 100;
    double val = 0.0;
    if (scanf("%lf", &val) != 1) {
        perror("Invalid input.\n");
    }
    printf("\nln(%lf)=%lf\n", val, CalcLn(val, seriesMembersAmount));
    return EXIT_SUCCESS;
}
