#include <stdlib.h>

void mat_alloc(double ***new_mat, unsigned int n, unsigned int m) {
	unsigned int i;

	double **tmp_mat;
	tmp_mat = (double**)malloc((n+2)*sizeof(double*));
	tmp_mat[0] = (double *)malloc((n+2)*(m+2)*sizeof(double));

	for (i=1; i<n+2; ++i) {
		tmp_mat[i] = tmp_mat[i-1] + (m+2);
	}

	*new_mat = tmp_mat;
}

void mat_free(double **new_mat, unsigned int n, unsigned int m) {	
	free(new_mat[0]);
	free(new_mat);
}

void mat_init(double **mat, unsigned int n, unsigned int m) {
	unsigned int i, j;

	for (i=0; i<n+2; ++i) {
		for (j=0; j<m+2; ++j) {
			mat[i][j] = 0;
			mat[i][j] = 0;
		}
	}

	for(j=0; j<m+2; ++j) {
		mat[0][j] = 100.0;
		mat[n+1][j] = 100.0;
		mat[0][j] = 100.0;
		mat[n+1][j] = 100.0;
	}

	for(i=0; i<n+2; ++i) {
		mat[i][0]   = 100.0;
		mat[i][m+1] = 100.0;
		mat[i][0]   = 100.0;
		mat[i][m+1] = 100.0;
	}

}
