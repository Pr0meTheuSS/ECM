#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <cmath>

double CalcLn(double x, int iterationAmount) {
	double reduceFactor = 1.0;
	const double suprIntervalValue = 0.05;
	while (std::abs( 1.0 - x) >= suprIntervalValue) {
		reduceFactor *= 2.0;
		x = sqrt(x);
	}
	// convert expr ln(X) to view ln(x + 1) and use x in series calculation
	x -= 1.0;
	double res = 0.0;
	double seriesMember = x;
	for (int i = 2; i < iterationAmount; i++) {
		res += seriesMember;
		seriesMember = -seriesMember * x / i;
	}
	return res * reduceFactor;
}
