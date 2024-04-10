#ifndef _MATRIX_H
#define _MATRIX_H

void mat_alloc(double ***new_mat, unsigned int n, unsigned int m);
void mat_free(double **new_mat, unsigned int n, unsigned int m);
void mat_init(double **mat, unsigned int n, unsigned int m);

#endif /* _MATRIX_H */
