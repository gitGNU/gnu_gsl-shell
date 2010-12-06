#ifndef ELLISS_H
#define ELLISS_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_vector_complex.h>

#include "disp-utils.h"

enum se_type {
  SE_ALPHA_BETA = 0,
  SE_PSI_DEL,
};

typedef enum {
  POL_S = 0,
  POL_P = 1
} polar_t;

struct elliss_ab {
  double alpha;
  double beta;
};

typedef struct elliss_ab  ell_ab_t[1];
typedef struct elliss_ab *ell_ab_ptr;

#define DEGREE(d) ((d) * M_PI / 180.0)

extern int
mult_layer_se_jacob (enum se_type type,
		     size_t nb, const cmpl ns[], double phi0, 
		     const double ds[], double lambda,
		     double anlz, ell_ab_t e,
		     gsl_vector *jacob_th, gsl_vector_complex *jacob_n);

#endif
