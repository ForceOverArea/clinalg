#pragma once
#include <stdlib.h>

#define mac(x, j, i) x->data[j]->data[i]

typedef struct {
	size_t len;
	long double data[];
} rowvec;

typedef struct {
	size_t rows;
	size_t cols;
	rowvec* data[];
} matrix;

rowvec* new_rowvec(size_t len);

void destroy_matrix(matrix* m);

matrix* new_matrix(size_t rows, size_t cols);

matrix* new_nxn(size_t n);

matrix* identity(size_t n);

void row_add(rowvec* a, rowvec* b, long double coef);

void print_rowvec(rowvec* r);

void print_matrix(matrix* m);

typedef struct {
	size_t len;
	long double* data[];
} pvec;

typedef struct {
	size_t rows;
	size_t cols;
	pvec* data[];
} ptrix;

ptrix* __from_matrix(matrix* m);

ptrix* __augment(matrix* a, matrix* b);

void destroy_ptrix(ptrix* p);

void __pvec_add(pvec* a, pvec* b, long double coef);

void print_pvec(pvec* p);

void print_ptrix(ptrix* p);

void __right_triangular(ptrix* p, size_t o);

void __left_triangular(ptrix* p, size_t o);

void __scale_diagonal(ptrix* p);

void reduce_ptrix(ptrix* p);

matrix* invert(matrix* m);