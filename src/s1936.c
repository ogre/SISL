/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/* (c) Copyright 1989,1990,1991,1992 by                                      */
/*     Senter for Industriforskning, Oslo, Norway                            */
/*     All rights reserved. See the copyright.h for more details.            */
/*                                                                           */
/*****************************************************************************/

#include "copyright.h"

/*
 *
 * $Id: s1936.c,v 1.1 1994-04-21 12:10:42 boh Exp $
 *
 */


#define S1936

#include "sislP.h"
#define MAX_SIZE  50



#if defined(SISLNEEDPROTOTYPES)
void
s1936 (SISLCurve * crv, double etd[], int ind, double *curvd, int *jstat)
#else
void
s1936 (crv, etd, ind, curvd, jstat)
     SISLCurve *crv;
     double etd[];
     int ind;
     double *curvd;
     int *jstat;

#endif
/*
*********************************************************************
*
*********************************************************************
*
* PURPOSE: To express a B-spline curve using a refined basis.
*	   of the refinement etref of the array et.
*
*
* INPUT:   crv	- The original curve.
*	   etd	- The knots of the subdivided curve.
*	   ind	- The number of vertices of the subdivided curve.
*
*
* OUTPUT:  curvd - Array containing the vertices of the refined curve.
*          jstat - Output status:
*                   < 0: Error.
*                   = 0: Ok.
*                   > o: Warning.
*
* METHOD:  The vertices are calculated using the "Oslo"-algorithm
*	   developped by Cohen, Lyche and Riesenfeld.
*
*
* REFERENCES: Cohen, Lyche, Riesenfeld: Discrete B-splines and subdivision
*	      techniques in computer aided geometric design, computer
*             graphics and image processing, vol 14, no.2 (1980)
*
* CALLS: s1937, s6err.
*
* WRITTEN BY :  Christophe R. Birkeland, SI, 1991-07
*
*********************************************************************
*/
{
  int kpos = 0;			/* Error position indicator	*/
  int ki, kj, kr, low;		/* Loop control parameters 	*/
  int nu;			/* Pointer into knot vector:
				 * knt[nu-1]<=etd[kj]<knt[nu]	*/
  int ins;			/* Number of vertices of curve	*/
  int iordr;			/* Order of curve		*/
  int idim;			/* Dimension of space where the
				 * curve lies			*/
  double *knt = NULL;		/* Original knot-vector. */
  double *coef = NULL;		/* Original coefficient arrays.	*/
  double sum;			/* Used to compute vertices of
				 * new curve			*/
  double sarray[MAX_SIZE];
  int alloc_needed=FALSE;
  double *alfa = NULL;		/* Array needed in subroutine
				 * s1937 (Oslo-algorithm)	*/

  *jstat = 0;


  /* Initialization. */

  knt = crv->et;
  ins = crv->in;
  iordr = crv->ik;
  idim = crv->idim;
  coef = crv->ecoef;


  /* Test if legal input. */

  if (iordr < 1)
    goto err110;
  if (ins < iordr || ind < iordr)
    goto err111;
  if (idim < 1)
    goto err102;


  /* Allocate array for internal use only. */

  if (iordr > MAX_SIZE)
    {
       if ((alfa = newarray (iordr, DOUBLE)) == NULL)
	 goto err101;
       alloc_needed = TRUE;
    }
  else
    alfa = sarray;

  /* Find if etd is a refinement of the original knot vector knt. */

  kj = 0;

  for (ki = 0; kj < ins; ki++)
    {
      if (ki >= ind)
	goto err111;
      if (knt[kj] > etd[ki])
	continue;
      if (knt[kj] < etd[ki])
	goto err112;
      kj++;
    }

  /* etd is a refinement of original knot vector knt
   * Produce refined curve. */

  nu = 1;
  for (kj = 0; kj < ind; kj++)
    {

      /* We want to find  knt[nu-1] <= etd[kj] < knt[nu]
         The copying of knots guarantees the nu-value to be found.
         Since kj is increasing, the nu-values will be increasing
         due to copying of knots. */

      for (; (((knt[nu - 1] > etd[kj]) || (etd[kj] >= knt[nu])) && (nu != ins));
	   nu++) ;

      /* Now we have  knt[nu-1] <= etd[kj] < knt[nu],
         so the discrete B-splines can be calculated. */

      s1937 (knt, iordr, kj + 1, nu, alfa, etd);


      /* Compute the coefficients of etd. */

      low = nu - iordr;
      for (ki = 0; ki < idim; ki++)
	{
	  sum = (double) 0.0;
	  for (kr = MAX (0, low); kr < nu; kr++)
	    sum += alfa[kr - low] * coef[kr * idim + ki];
	  curvd[kj * idim + ki] = sum;
	}
    }

  /* OK. */

  goto out;


  /* Memory error */

err101:
  *jstat = -101;
  s6err ("s1936", *jstat, kpos);
  goto out;

  /* Error in B-spline curve description:
     Dimension less than 1. */

err102:
  *jstat = -102;
  s6err ("s1936", *jstat, kpos);
  goto out;

  /* Order less than 1. */

err110:
  *jstat = -110;
  s6err ("s1936", *jstat, kpos);
  goto out;

  /* No. of vertices less than order. */

err111:
  *jstat = -111;
  s6err ("s1936", *jstat, kpos);
  goto out;

  /* Error in knot-vector. */

err112:
  *jstat = -112;
  s6err ("s1936", *jstat, kpos);

out:
  if (alloc_needed)
    freearray (alfa);

  return;
}
#undef MAX_SIZE
