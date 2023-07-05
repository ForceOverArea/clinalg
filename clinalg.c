#include <stdlib.h>
#include <stdio.h>


typedef struct {
	size_t len;
	long double data[];
} rowvec;

typedef struct {
	size_t rows;
	size_t cols;
	rowvec *data[];
} matrix;

/*
Creates a new row vector pointer with all values initialized to 0.
This operation is O(n) w.r.t. len of the vec.
*/
rowvec *new_rowvec(size_t len) {
	rowvec *p = (rowvec *)malloc(sizeof(rowvec) + sizeof(long double) * len);
	if (p != NULL) {
		p->len = len;
		for (int i = 0; i < (int)len; i++) {
			p->data[i] = 0; // initialize to 0
		}
	}
	return p;
}

/*
Private function to free all rows in a matrix. This function should only be used in 
matrix constructor functions to free them in the event that one or more rows cannot 
successfully be malloced.
*/
void destroy_matrix(matrix* m) {
	for (int i = 0; i < (int)m->rows; i++) {
		free(m->data[i]); // free row
		m->data[i] = NULL;
	}
	free(m);
	m = NULL;
}

/*
* Creates a new `m x n` matrix with the specified number of rows 
* and columns. This operation is O(n * m) w.r.t. rows and cols.
*/
matrix *new_matrix(size_t rows, size_t cols) {
	matrix *m = (matrix *)malloc(sizeof(matrix) + sizeof(rowvec *) * rows);
	if (m != NULL) {
		m->rows = 0;
		m->cols = cols;
		for (int i = 0; i < (int)rows; i++) {
			rowvec *r = new_rowvec(cols);
			if (r == NULL) { 
				destroy_matrix(m); // free other rows if desired rows can't be malloced
				return NULL;
			}
			m->data[i] = r;
			m->rows++; // only increment once the row has successfully been added.
		}
	}
	return m;
}

/*
Creates a new `n x n` matrix. This operation is O(n^2) w.r.t. n
*/
matrix *new_nxn(size_t n) {
	return new_matrix(n, n);
}

/*
Creates an `n x n` identity matrix. This operation is O(n^2).
*/
matrix *identity(size_t n) {
	matrix *m = (matrix *)malloc(sizeof(matrix) + sizeof(rowvec*) * n);
	if (m != NULL) {
		m->rows = 0;
		m->cols = n;
		for (int j=0; j < (int)n; j++) {
			
			// create new row or clean up if impossible
			rowvec* r = (rowvec*)malloc(sizeof(rowvec) + sizeof(long double) * n);
			if (r == NULL) {
				destroy_matrix(m);
				return NULL;
			}
			m->data[j] = r;
			m->rows++;
			m->data[j]->len = n;

			// populate cells of matrix
			for (int i = 0; i < (int)n; i++) {
				if (i == j) {
					m->data[j]->data[i] = 1;
				} else {
					m->data[j]->data[i] = 0;
				}
			}
		}
	}
	return m;
}

/*
Adds pvec `b` scaled by `coef` to pvec `a`.
*/
void row_add(rowvec* a, rowvec* b, long double coef) {
	if (a->len != b->len)
		puts("error: row vectors must have equal len");
	else
		for (int i = 0; i < a->len; i++)
			a->data[i] += b->data[i] * coef;
}

void print_rowvec(rowvec* r) {
	printf("[");
	for (int i = 0; i < r->len-1; i++) {
		printf("%Lf, ", r->data[i]);
	}
	printf("%Lf]\n", r->data[r->len-1]);
}

void print_matrix(matrix* m) {
	printf("[\n");
	for (int i = 0; i < m->rows; i++) {
		printf(" ");
		print_rowvec(m->data[i]);
	}
	printf("]\n");
}

typedef struct {
	size_t len;
	long double* data[];
} pvec;

typedef struct {
	size_t rows;
	size_t cols;
	pvec* data[];
} ptrix;

/*Creates a ptrix (matrix w/ pointers to another matrix's values) from a 
given matrix pointer.*/
ptrix* __from_matrix(matrix* m) {
	ptrix* p = (ptrix*)malloc(sizeof(ptrix) + sizeof(pvec*) * m->rows);
	if (p == NULL) {
		puts("insufficient heap memory for new ptrix");
		return NULL;
	}
	p->cols = m->cols;
	p->rows = m->rows;

	for (int j = 0; j < (int)m->rows; j++) {
		pvec* pv = (pvec*)malloc(sizeof(pvec) + sizeof(long double*) * m->cols);
		if (pv == NULL) {
			destroy_matrix((matrix*)p);
			return NULL;
		}
		p->data[j] = pv;
		p->data[j]->len = p->cols;
		for (int i = 0; i < (int)m->cols; i++) {
			p->data[j]->data[i] = &m->data[j]->data[i]; // make references to m's indices
		}
	}

	return p;
}

/*Combines two matrices into a ptrix with references to each
matrix's elements. This appends the rows of b to the rows of a.
Both matrices passed to this function must have the same number of 
rows in order for this function to work.

This function returns NULL if the augment matrix cannot be created 
for any reason.*/
ptrix* __augment(matrix* a, matrix* b) {

	if (a->rows != b->rows) {
		puts("matrices must have same number or rows");
		return NULL;
	}

	ptrix* p = (ptrix*)malloc(sizeof(ptrix) + sizeof(pvec*) * a->rows);
	if (p == NULL) {
		puts("insufficient heap memory for new augment ptrix");
		return NULL;
	}
	p->rows = a->rows;
	p->cols = a->cols + b->cols;

	for (int j = 0; j < (int)p->rows; j++) {
		pvec* pv = (pvec*)malloc(sizeof(pvec) + sizeof(long double*) * p->cols);
		if (pv == NULL) {
			destroy_matrix((matrix*)p);
			puts("insufficient heap memory for one or more ptrix rows, aborting ptrix creation.");
			return NULL;
		}
		p->data[j] = pv;
		p->data[j]->len = p->cols;
		for (int i = 0; i < (int)p->cols; i++) {
			if (i < a->cols) {
				p->data[j]->data[i] = &a->data[j]->data[i]; // make references to m's indices
			} else {
				p->data[j]->data[i] = &b->data[j]->data[i - (a->cols)];
			}
		}
	}

	return p;
}

void destroy_ptrix(ptrix* p) {
	destroy_matrix((matrix*)p);
}

void print_pvec(pvec* p) {
	//printf("%d\n", (int)p->len);
	printf("[");
	for (int i = 0; i < p->len-1; i++) {
		printf("%Lf, ", *p->data[i]);
	}
	printf("%Lf]\n", *p->data[p->len-1]);
}

void __pvec_add(pvec* a, pvec* b, long double coef) {
	
	// printf("Adding ");
	// print_pvec(b);
	// printf("to     ");
	// print_pvec(a);
	// printf("... * %Lf\n", coef);
	
	if (a->len != b->len)
		puts("error: row pointer vectors must have equal len");
	else
		for (int i = 0; i < a->len; i++)
			*a->data[i] += *b->data[i] * coef;
}

void print_ptrix(ptrix* p) {
	puts("[");
	for (int i = 0; i < p->rows; i++) {
		printf(" ");
		print_pvec(p->data[i]);
	}
	printf("]\n");
}

void __right_triangular(ptrix* p, size_t o) {
	if (o == p->rows-1)
		return;

	long double headhead = *p->data[o]->data[o];
	for (int j = o + 1; j < p->rows; j++) {
	
		long double rowhead = *p->data[j]->data[o];
		__pvec_add(p->data[j], p->data[o], -rowhead / headhead);
	}

	__right_triangular(p, ++o); // recurse on minor of 0,0 
}

void __left_triangular(ptrix* p, size_t o) {
	if (o == 0)
		return;

	long double tailtail = *p->data[o]->data[o];
	for (int j = o - 1; j >= 0; j--) {

		long double rowtail = *p->data[j]->data[o];
		__pvec_add(p->data[j], p->data[o], -rowtail / tailtail);
	}

	__left_triangular(p, --o); // recurse on minor of n,n 
}

void __scale_diagonal(ptrix* p) {

	for (int j = 0; j < p->rows; j++) {
		long double scalar = *p->data[j]->data[j];
	
		for (int i = 0; i < p->cols; i++) {

			*p->data[j]->data[i] /= scalar;
		}
	}
}

void reduce_ptrix(ptrix *p) {
	__right_triangular(p, 0);
	__left_triangular(p, p->rows-1);
	__scale_diagonal(p);
}

matrix* invert(matrix* m) {
	matrix* res = identity(m->rows);
	ptrix* p = __augment(m, res);
	reduce_ptrix(p);
	destroy_matrix(m);
	return res;
}
