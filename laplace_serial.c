#include <stdio.h>
#include <math.h>
#include "matrix.h"



#define EPSILON   (0.001)
#define N         (1024)
#define M         (1024)
#define MAX_ITER  (100000)
#define RES_STEPS (100)



double calc_residual(double **matNewValues, double **matOldValues) 
{
	double dResiduum=-1;
	double temp;
	for (int i=1; i<N+1; i++) {
		for (int j=1; j<M+1; j++) {
			
			temp=fabs(matNewValues[i][j]-matOldValues[i][j]);
			if (temp > dResiduum) {
				dResiduum=temp;
				
			}
		}
	}
	return dResiduum;
}

void init(double ***matNewValues,  double ***matOldValues) {
	mat_alloc(matNewValues, N, M);    
	mat_alloc(matOldValues, N, M);    
	mat_init(*matNewValues, N, M);
	mat_init(*matOldValues, N, M);
}

int main(int argc, char* argv[])
{
	int i,j,t;
	double **matOldValues;
	double **matNewValues;
	double dResiduum;
	
	
	

	/* Initialisierung der Matrizen */
	init(&matNewValues, &matOldValues);
	
	
	
	/* Start der Laplace-Berechnung  */
	for (t=0; t<MAX_ITER; t++) {

		for (i=1; i<N+1; i++) {
			for (j=1; j<M+1; j++) {
				matNewValues[i][j]= 0.25*(matOldValues[i-1][j] + 
				                          matOldValues[i+1][j] + 
				                          matOldValues[i][j-1] + 
				                          matOldValues[i][j+1]);
			}
		}  


		if (t % RES_STEPS == 0) {
			dResiduum = calc_residual(matNewValues, matOldValues);

			if (dResiduum <= EPSILON) {
				t++;
				break;
			}
		}

		/* Austausch der Matrizen */
		double** old = matOldValues;
		matOldValues = matNewValues;
		matNewValues = old;
	}
	
	
	
	
	
	

	return 0;
}

