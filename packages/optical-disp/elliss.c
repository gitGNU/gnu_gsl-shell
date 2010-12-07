
#include "elliss.h"

static inline cmpl
csqr(cmpl x)
{
  return x*x;
}

static cmpl
refl_coeff (cmpl nt, cmpl cost, cmpl nb, cmpl cosb, polar_t pol)
{
  cmpl rc;

  if (pol == POL_P)
    rc = (nb*cost - nt*cosb) / (nb*cost + nt*cosb);
  else
    rc = (nt*cost - nb*cosb) / (nt*cost + nb*cosb);
  
  return rc;
}

/* NB: drdnt given by this procedure is conceptually wrong if the
   medium of "nt" is the topmost medium (environment). That's because
   we assume that the angle of incidence is fixed a priori. */
static cmpl
refl_coeff_ext (cmpl nt, cmpl cost,
		cmpl nb, cmpl cosb,
		cmpl * drdnt, cmpl * drdnb, polar_t pol)
{
  cmpl rc;

  if (pol == POL_P)
    {
      cmpl den = nb*cost + nt*cosb;
      cmpl isqden = 1 / csqr(den);
      rc = (nb*cost - nt*cosb) / den;
      *drdnt = - 2.0 * cosb * nb * (2*cost*cost-1) * isqden / cost;
      *drdnb =   2.0 * cost * nt * (2*cosb*cosb-1) * isqden / cosb;
    }
  else
    {
      cmpl den = nt*cost + nb*cosb;
      cmpl isqden = 1 / csqr(den);
      rc = (nt*cost - nb*cosb) / den;
      *drdnt =   2.0 * cosb * nb * isqden / cost;
      *drdnb = - 2.0 * cost * nt * isqden / cosb;
    }

  return rc;
}

static cmpl
snell_cos (cmpl nsin0, cmpl nlyr)
{
  cmpl s = nsin0 / nlyr;
  return csqrt(1.0 - csqr(s));
}

static void
mult_layer_refl (int nb, const cmpl ns[], int ns_stride, cmpl nsin0, 
		 const double ds[], int ds_stride, double lambda, cmpl R[])
{
  const double omega = 2 * M_PI / lambda;
  cmpl cosc, cost;
  const cmpl *nptr;
  cmpl n0, n1;
  int j;

  /* In this procedure we assume (nb > 2). This condition should be
     ensured in advance. */

  nptr = ns + (nb-2) * ns_stride;

  n0 = *nptr;
  n1 = *(nptr + ns_stride);

  cost = snell_cos (nsin0, n0);
  cosc = snell_cos (nsin0, n1);

  R[0] = refl_coeff (n0, cost, n1, cosc, POL_S);
  R[1] = refl_coeff (n0, cost, n1, cosc, POL_P);

  for (j = nb - 3; j >= 0; j--)
    {
      cmpl r[2], rho, beta;
      double th = *(ds + j * ds_stride);
      polar_t p;

      nptr -= ns_stride;

      n0 = *nptr;
      n1 = *(nptr + ns_stride);
 
      cosc = cost;
      cost = snell_cos (nsin0, n0);

      beta = - 2.0 * I * omega * n1 * cosc;
      rho = cexp(beta * th);

      for (p = 0; p <= 1; p++)
	{
	  r[p] = refl_coeff (n0, cost, n1, cosc, p);

	  R[p] = (r[p] + R[p] * rho) / (1 + r[p] * R[p] * rho);
      }
    }
}

static void
mult_layer_refl_jacob_th (int nb, const cmpl ns[], int ns_stride, cmpl nsin0, 
			  const double ds[], int ds_stride, double lambda, 
			  cmpl R[], cmpl *jacth)
{
  const double omega = 2 * M_PI / lambda;
  const int nblyr = nb - 2;
  cmpl cosc, cost;
  const cmpl *nptr; 
  cmpl n0, n1;
  int j;

  /* In this procedure we assume (nb > 2). This condition should be
     ensured in advance. */

  nptr = ns + (nb-2) * ns_stride;

  n0 = *nptr;
  n1 = *(nptr + ns_stride);

  cost = snell_cos (nsin0, n0);
  cosc = snell_cos (nsin0, n1);

  R[0] = refl_coeff (n0, cost, n1, cosc, POL_S);
  R[1] = refl_coeff (n0, cost, n1, cosc, POL_P);

  for (j = nb - 3; j >= 0; j--)
    {
      cmpl r[2], rho, beta, drhodth;
      double th = *(ds + j * ds_stride);
      polar_t p;
      int k;

      nptr -= ns_stride;

      n0 = *nptr;
      n1 = *(nptr + ns_stride);

      cosc = cost;
      cost = snell_cos (nsin0, n0);

      beta = - 2.0 * I * omega * n1 * cosc;
      rho = cexp(beta * th);
      drhodth = rho * beta;

      for (p = 0; p <= 1; p++)
	{
	  cmpl dfdR, dfdrho;
	  cmpl *pjacth = jacth + (p == 0 ? 0 : nblyr);
	  cmpl den, isqden;

	  r[p] = refl_coeff (n0, cost, n1, cosc, p);

	  den = 1 + r[p] * R[p] * rho;
	  isqden = 1 / csqr(den);
	  dfdR = rho * (1 - r[p]*r[p]) * isqden;

	  for (k = nblyr; k > j+1; k--)
	    pjacth[k-1] *= dfdR;

	  dfdrho = R[p] * (1 - r[p]*r[p]) * isqden;

	  pjacth[j] = dfdrho * drhodth;

	  R[p] = (r[p] + R[p] * rho) / den;
      }
    }
}

static void
mult_layer_refl_jacob (int nb, 
		       const cmpl ns[], int ns_stride, cmpl nsin0, 
		       const double ds[], int ds_stride, 
		       double lambda, cmpl R[],
		       cmpl *jacth, cmpl *jacn)
{
  const double omega = 2 * M_PI / lambda;
  const int nblyr = nb - 2;
  cmpl cosc, cost;
  const cmpl *nptr;
  cmpl drdnt[2], drdnb[2];
  cmpl n0, n1;
  int j;

  /* In this procedure we assume (nb > 2). This condition should be
     ensured in advance. */

  nptr = ns + (nb-2) * ns_stride;

  n0 = *nptr;
  n1 = *(nptr + ns_stride);

  cost = snell_cos (nsin0, n0);
  cosc = snell_cos (nsin0, n1);

  R[0] = refl_coeff_ext (n0, cost, n1, cosc, &drdnt[0], &drdnb[0], POL_S);
  R[1] = refl_coeff_ext (n0, cost, n1, cosc, &drdnt[1], &drdnb[1], POL_P);

  jacn[nb-1]      = drdnb[0];
  jacn[nb + nb-1] = drdnb[1];

  jacn[nb-2]      = drdnt[0];
  jacn[nb + nb-2] = drdnt[1];

  for (j = nb - 3; j >= 0; j--)
    {
      cmpl r[2], rho, beta, drhodn, drhodth;
      double th = *(ds + j * ds_stride);
      polar_t p;
      int k;

      nptr -= ns_stride;

      n0 = *nptr;
      n1 = *(nptr + ns_stride);

      cosc = cost;
      cost = snell_cos (nsin0, n0);

      beta = - 2.0 * I * omega * n1 * cosc;
      rho = cexp(beta * th);
      drhodth = rho * beta;
      drhodn = - 2.0 * I * rho * omega * th / cosc;

      for (p = 0; p <= 1; p++)
	{
	  cmpl dfdR, dfdr, dfdrho;
	  cmpl *pjacn  = jacn  + (p == 0 ? 0 : nb);
	  cmpl *pjacth = jacth + (p == 0 ? 0 : nblyr);
	  cmpl den, isqden;

	  r[p] = refl_coeff_ext (n0, cost, n1, cosc, &drdnt[p], &drdnb[p], p);

	  den = 1 + r[p] * R[p] * rho;
	  isqden = 1 / csqr(den);
	  dfdR = rho * (1 - r[p]*r[p]) * isqden;

	  for (k = nb - 1; k > j+1; k--)
	    pjacn[k] *= dfdR;

	  for (k = nblyr; k > j+1; k--)
	    pjacth[k-1] *= dfdR;

	  dfdr = (1 - csqr(R[p]*rho)) * isqden;
	  dfdrho = R[p] * (1 - r[p]*r[p]) * isqden;

	  pjacn[j+1] = dfdR * pjacn[j+1] + dfdr * drdnb[p] + dfdrho * drhodn;
	  pjacn[j] = (j == 0 ? 0.0 : dfdr * drdnt[p]);

	  pjacth[j] = dfdrho * drhodth;

	  R[p] = (r[p] + R[p] * rho) / den;
      }
    }
}

/* NB: In this case we are treating a Psi-Delta spectrum and
   the fields named alpha and beta corresponds actually to
   tan(psi) and cos(delta), respectively. */
static void
se_psidel (cmpl R[], ell_ab_t e)
{
  cmpl rho = R[1] / R[0];
  e->alpha = cabs(rho);
  e->beta  = creal(rho) / e->alpha;
}

static void
se_psidel_der (cmpl R[], cmpl dR[], cmpl *dtpsi, cmpl *dcdelta)
{
  cmpl rho = R[1] / R[0];
  cmpl drho = (R[0]*dR[1] - R[1]*dR[0]) / (R[0]*R[0]);
  double irhosq = 1 / CSQABS(rho);
  double iden = sqrt(irhosq);

  *dtpsi = iden * conj(rho) * drho;
  *dcdelta = iden * (1 - conj(rho) / rho) * drho / 2.0;
}

static void
se_ab (cmpl R[], double tanlz, ell_ab_t e)
{
  cmpl rho = R[1] / R[0];
  double sqtpsi = CSQABS(rho);
  double tasq = tanlz * tanlz;
  double iden = 1 / (sqtpsi + tasq);
  e->alpha = (sqtpsi - tasq) * iden;
  e->beta = 2 * creal(rho) * tanlz * iden;
}

static void
se_ab_der (cmpl R[], cmpl dR[], double tanlz, cmpl *dalpha, cmpl *dbeta)
{
  cmpl rho = R[1] / R[0];
  cmpl drho = (R[0]*dR[1] - R[1]*dR[0]) / (R[0]*R[0]);
  double sqtpsi = CSQABS(rho);
  double tasq = tanlz * tanlz;
  double isqden;
  cmpl z;

  isqden = 1 / (sqtpsi + tasq);
  isqden *= isqden;

  z = conj(rho) * drho;
  *dalpha = 4 * tasq * z * isqden;

  z = conj(tasq - rho*rho) * drho;
  *dbeta = 2 * tanlz * z * isqden;
}

int
mult_layer_se_jacob (enum se_type type, size_t _nb,
		     const cmpl *ns, int ns_stride, double phi0, 
		     const double *ds, int ds_stride, double lambda,
		     double anlz, ell_ab_t e,
		     gsl_vector *jacob_th, gsl_vector_complex *jacob_n)
{
#define NB_JAC_STATIC 10
  static struct { cmpl th[2*NB_JAC_STATIC], n[2*NB_JAC_STATIC]; } jacs;
  struct { cmpl *th, *n; } jac;
  const int nb = _nb, nblyr = nb - 2;
  int use_static = (nb <= NB_JAC_STATIC);
  double tanlz = tan(anlz);
  cmpl R[2], nsin0;
  cmpl *jbuffer;
  size_t j;

  if (use_static)
    {
      jac.th = jacs.th;
      jac.n  = jacs.n;
    }
  else
    {
      jbuffer = malloc (2 * 2*nb*sizeof(cmpl));
      if (jbuffer == 0)
	return 0;
      jac.th = jbuffer;
      jac.n  = jbuffer + 2*nb;
    }

  nsin0 = ns[0] * csin ((cmpl) phi0);

  if (jacob_th && jacob_n)
    mult_layer_refl_jacob (nb, ns, ns_stride, nsin0, ds, ds_stride, lambda, 
			   R, jac.th, jac.n);
  else if (jacob_th)
    mult_layer_refl_jacob_th (nb, ns, ns_stride, nsin0, ds, ds_stride, lambda, 
			      R, jac.th);
  else
    mult_layer_refl (nb, ns, ns_stride, nsin0, ds, ds_stride, lambda, R);

  if (type == SE_ALPHA_BETA)
    se_ab (R, tanlz, e);
  else
    se_psidel (R, e);

  if (jacob_n)
    {
      /* we set the derivative respect to the RI of the ambient to 0.0 */
      vec_complex_set (jacob_n, 0, 0.0i);
      vec_complex_set (jacob_n, nb, 0.0i);
      for (j = 1; j < (size_t) nb; j++)
	{
	  cmpl dR[2] = {jac.n[j], jac.n[nb+j]};
	  cmpl d_alpha, d_beta;

	  if (type == SE_ALPHA_BETA)
	    se_ab_der (R, dR, tanlz, &d_alpha, &d_beta);
	  else
	    se_psidel_der (R, dR, &d_alpha, &d_beta);

	  vec_complex_set (jacob_n, j,    d_alpha);
	  vec_complex_set (jacob_n, nb+j, d_beta);
	}
    }

  if (jacob_th)
    {
      for (j = 0; j < (size_t) nblyr; j++)
	{
	  cmpl dR[2] = {jac.th[j], jac.th[nblyr+j]};
	  cmpl d_alpha, d_beta;

	  if (type == SE_ALPHA_BETA)
	    se_ab_der (R, dR, tanlz, &d_alpha, &d_beta);
	  else
	    se_psidel_der (R, dR, &d_alpha, &d_beta);

	  gsl_vector_set (jacob_th, j,       creal(d_alpha));
	  gsl_vector_set (jacob_th, nblyr+j, creal(d_beta));
	}
    }

  if (! use_static)
    free (jbuffer);

  return 1;
}
