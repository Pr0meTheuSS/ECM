#define _CRT_SECURE_NO_WARNINGS
#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define CIRCLE_COUNTER 6400000
using namespace std;

double McLorenExp(double x, int N) {	
	double result = 0.0;
	double element = 1.0;
	for (int i = 0; i < N; i++) {
		result += element;
		element = element * x / (i + 1);
	}
	return result;
};

int main(void) { 
	
	double * arr = (double *)malloc(CIRCLE_COUNTER * sizeof(double));
	
	int Start = clock();
	
	for (int i = 0; i < CIRCLE_COUNTER; i++)
		arr[i] = McLorenExp(1.0/(double)i, 1000);		
	
	int Finish = clock();
	
	printf("Result time:\t%f",  ((float)(Finish - Start) / (float)CLK_TCK) /*/CIRCLE_COUNTER*/);
	
	FILE * out = fopen("out.txt", "wb");
	
	for(int i = 0; i < CIRCLE_COUNTER; i++)
		fprintf(out, "%lf", arr[i]);
	free(arr);
	fclose(out);
	return 0;
}

