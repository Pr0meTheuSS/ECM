#include "CalcLn.h"

double CalcLn(double x, size_t iterationAmount) {
	double reduceFactor = 1.0;
	const double suprIntervalValue = 0.05;
	while (abs( 1.0 - x) >= suprIntervalValue) {
		reduceFactor *= 2.0;
		x = sqrt(x);
	}
	// convert expr ln(X) to view ln(x + 1) and use x in series calculation
	x = x - 1.0;
	double res = 0.0;
	double seriesMember = x;
	for (size_t i = 2; i < iterationAmount; i++) {
		res += seriesMember;
		seriesMember = -seriesMember * x / i;
	}
	return res * reduceFactor;
}
