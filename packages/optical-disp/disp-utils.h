#ifndef DISP_UTILS_H
#define DISP_UTILS_H

#include <math.h>
#include <complex.h>
#include <gsl/gsl_vector.h>

typedef double complex cmpl;

extern void vec_complex_set (gsl_vector_complex *v, int i, cmpl val);
extern cmpl vec_complex_get (gsl_vector_complex *v, int i);

#define emalloc(n) erealloc(NULL, n);
extern void * erealloc (void *p, int n);

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419716939937510
#endif

#define CSQABS(z) (creal(z)*creal(z) + cimag(z)*cimag(z))

#endif
