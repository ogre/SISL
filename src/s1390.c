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
 * $Id: s1390.c,v 1.1 1994-04-21 12:10:42 boh Exp $
 *
 */


#define S1390

#include "sislP.h"

#if defined(SISLNEEDPROTOTYPES)
void 
s1390 (SISLCurve * pc1[], SISLSurf ** ps1, int nder[], int *jstat)
#else
void 
s1390 (pc1, ps1, nder, jstat)
     SISLCurve *pc1[];
     SISLSurf **ps1;
     int nder[];
     int *jstat;
#endif
/*
*********************************************************************
*
*********************************************************************
*
*  Purpose : Make a 4-edged-blending surface between 4 curves
*            where each curve is associated a number of cross-derivative
*            curves. The curves are numbered successively around the
*            blending parameter directions are expected to be as follows when
*            this routine is entered:
*
*                                 Direction right
*                                      3
*                                 ------------
*                                 !          !
*                    Direction up !          ! Direction up
*                         4       !          !      2
*                                 !          !
*                                 !          !
*                                 ------------
*                                 Direction rigth
*                                      1
*
*
*       NB!  The cross-derivatives are always pointing into the patch,
*            and note the directions in the above diagram.
*
*
*
* Input     : pc1       - Pointers to boundary curves
*                         pc1[i],i=0,...nder[0]-1 are pointers to position
*                         and cross-derivatives along first edge.
*                         pc1[i],i=nder[0],...nder[1]-1 are pointers
*                         to position and cross-derivatives along second edge.
*                         pc1[i],i=nder[0]+nder[1],...nder[2]-1 are pointers
*                         to position and cross-derivatives along third edge.
*                         pc1[i],i=nder[0]+nder[1]+nder[2],...nder[3]-1 are
*                         pointers to position and cross-derivatives
*                         along fourth edge.
*             nder[0:3] - nder[i] gives number of curves on
*                         edge number i+1.
*
* Output    : ps1       - Pointer to blending surface.
*
*             jstat     - status messages
*                                         > 0      : warning
*                                         = 0      : ok
*                                         < 0      : error
*
* Use       : SISLCurve pc1[ANT];
*             SISLSurf  *ps1;
*             int jstat,nder[4];
*
*             pc1[0] = curve one;
*             .
*             .
*             s1390(pc1,&ps1,nder,&jstat);
*
*
* NB!    1. Inconsistent corner information will be overruled.
*
*-
* Calls        : Previous version called h19732 - Rectangular blending.
*		 This version calls s1750,s1333,s1387,
*		      s1934,s1935,s1938,s1924,s6err
*
* Note	       : The original code written by Morten was removed and
*                replaced by the code in s1922, and s1922.c was then
*                removed. This was done by Bjoern Olav Hoset.
*
* Written by   : Morten Daehlen, SI, Aug. 88.
* Rewritten by : Christophe R. Birkeland, SI, Aug. 91.(Routine s1922)
* Revised by:    Christophe Rene Birkeland, SI, May. 93.
*
*********************************************************************
*/
{
  int kstat = 0;		/* Status variable used in calls to
				 * lower subroutines			*/
  int kpos = 0;			/* Error position indicator		*/
  int idim;			/* Dimension of the space in which the
				 * curves lie				*/
  int ki, kj, kk, kij;		/* Loop control parameter		*/
  int kidim;			/* Equals ki*idim			*/
  int kjdim;			/* Equals kj*idim			*/
  int n0, n1, n2, n3;		/* Equals nder[0],nder[1],nder[2],nder[3]*/
  int n01;			/* Equals nder[0]+nder[1]		*/
  int n012;			/* Equals nder[0]+nder[1]+nder[2]	*/
  int inbcrv13;			/* Number of curves in vpcrv13		*/
  int inbcrv24;			/* Number of curves in vpcrv24		*/
  int iopen = TRUE;		/* Indicates open=TRUE or closed curve	*/
  int ord13 = 0;		/* Order to be used in lofting following
				 * curves 1 and 3                       */
  int ord24 = 0;		/* Order to be used in lofting following
				 * curves 1 and 3                       */
  int order2;		        /* Order of lofted surfaces in second
				 * parameter direction                  */
  int nr1;			/* Number of degrees of freedom in the
				 * B-basis given by the knot vector tr1 */
  int nr2;			/* Number of degrees of freedom in the
				 * B-basis given by the knot vector tr2 */
  int in2;			/* Equals crv2ki->in			*/
  int in3;			/* Equals crv3ki->in			*/
  int sf1ind, sf2ind;		/* Index used in algorithm to match
				 * vertices from surface 1 & 2		*/
  int type[6];			/* Indicates type of curve in curve-set */
  double stpar = DNULL;		/* Start parameter value only used in
				 * call s1333.				*/
  double ewrem;			/* Stores a value of array ew		*/
  double start = 0.0;	        /* Start and stop values in normalized 	*/
  double stop = 1.0;    	/* knot vectors				*/
  double *tr1 = NULL;		/* Union of knot-vectors nsurf13->et1
				 * and nsurf24->et2			*/
  double *tr2 = NULL;		/* Union of knot-vectors nsurf13->et2
				 * and nsurf24->et1			*/
  double *cpar = NULL;		/* Array needed in call s1333		*/
  double *srfr1 = NULL;		/* Vertices of first surface represented
				 * with common refined knot vectors	*/
  double *srfr2 = NULL;		/* Vertices of second surface represented
				 * with common refined knot vectors	*/
  double *scoef = NULL;		/* Vertices of output surface		*/
  double *ew = NULL;		/* Weight matrix used used to blend
				 * together 2 surfaces			*/
  SISLCurve *rc1 = NULL;	/* Parameter needed in call s1750	*/
  SISLCurve *rc2 = NULL;	/* Parameter needed in call s1750	*/
  SISLCurve *vpcrv13[6];	/* Array of pointers to curves 1 & 3
				 * (inclusive derivatives)		*/
  SISLCurve *vpcrv24[6];	/* Array of pointers to curves 2 & 4
				 * (inclusive derivatives)		*/
  SISLCurve *crv2ki = NULL;	/* "Derivatiave curve" ki-1 of curve 2	*/
  SISLCurve *crv3ki = NULL;	/* "Derivatiave curve" ki-1 of curve 3	*/
  SISLSurf *surf13 = NULL;	/* Surface generated by s1333 from curves
				 * contained in vpcrv13			*/
  SISLSurf *nsurf13 = NULL;	/* Surface generated by s1387		*/
  SISLSurf *surf24 = NULL;	/* Surface generated by s1333 from curves
				 * contained in vpcrv24			*/
  SISLSurf *nsurf24 = NULL;	/* Surface generated by s1387		*/

  *jstat = 0;


  /* 
   * Initialization
   * ---------------
   */

  idim = pc1[0]->idim;
  n0   = nder[0];
  n1   = nder[1];
  n2   = nder[2];
  n3   = nder[3];
  n01  = n0 + n1;
  n012 = n01 + n2;


  /* 
   * Check input: derivative order and all input curves
   * --------------------------------------------------
   */

  if (n0 > 3 || n1 > 3 || n2 > 3 || n3 > 3)
    goto err151;
  for(ki=0; ki < n012+n3; ki++)
    {
      s1707(pc1[ki], &kstat);
      if (kstat < 0) goto error;
    }


  /*
   * Copy curves 1 & 3 (inclusive derivatives) into vpcrv13
   * ------------------------------------------------------
   */

  inbcrv13 = n0 + n2;

  for (ki = 0; ki < n0; ki++)
    vpcrv13[ki] = pc1[ki];
  for (ki = 0; ki < n2; ki++)
    vpcrv13[ki + n0] = pc1[n01 + ki];

  /*
   * Copy curves 2 & 4 (inclusive derivatives) into vpcrv24
   * ------------------------------------------------------
   */

  inbcrv24 = n1 + n3;

  for (ki = 0; ki < n3; ki++)
    vpcrv24[ki] = pc1[n012 + ki];
  for (ki = 0; ki < n1; ki++)
    vpcrv24[ki + n3] = pc1[n0 + ki];


  /*
   * Find max. order in lofting directions
   * ----------------------------------------------------
   */

  for(ki=0; ki < n0+n2; ki++)
    ord13 = MAX( vpcrv13[ki]->ik, ord13);
  ord13 = MAX( n1+n3, ord13);

  for(ki=0; ki < n1+n3; ki++)
    ord24 = MAX( vpcrv24[ki]->ik, ord24);
  ord24 = MAX( n0+n2, ord24);


  /* 
   * Spline lofted surface between curve 1 and 3.
   * --------------------------------------------
   */

  for (ki = 0; ki < n0; ki++)
    switch (ki)
      {
      case 0:
	type[ki] = 1;
	continue;
      case 1:
	type[ki] = 4;
	continue;
      case 2:
	type[ki] = 6;
	continue;
      }
  for (ki = 0; ki < n2; ki++)
    {
      switch (ki)
	{
	case 0:
	  type[ki + n0] = 1;
	  continue;
	case 1:
	  type[ki + n0] = 4;
	  break;
	case 2:
	  type[ki + n0] = 6;
	  break;
	}

      if (ki > 0)
	{
	  /* 
	   * Derivative directions along curve 3
	   * must be turned for use in call s1333 
	   * -------------------------------------
	   */

	  crv3ki = vpcrv13[n0 + ki];
	  in3 = crv3ki->in;
	  for (kj = 0; kj < idim * in3; kj++)
	    crv3ki->ecoef[kj] = -crv3ki->ecoef[kj];
	}
    }

  /* 
   * LOFTING in 1. par. dir. using curve 1 and 3
   * -------------------------------------------
   */

  s1333 (inbcrv13, vpcrv13, type, stpar, iopen, ord24, 0,
	 &surf13, &cpar, &kstat);
  if (kstat < 0) goto error;

  /* 
   * DEGREE raising if  necessary, first check that
   * order in 1. par. direction is right.
   * ----------------------------------------------
   */

  order2 = MAX( surf13->ik2, ord24 );

  if (surf13->ik1 != ord13) goto err160;

  if (order2 > surf13->ik2)
    s1387 (surf13, ord13, order2, &nsurf13, &kstat);
  else
    nsurf13 = surf13;
  if (kstat < 0) goto error;

  /* 
   * Derivative direction along curve 3
   * must be turned back to original direction 
   * ------------------------------------------
   */

  for (ki = 1; ki < n2; ki++)
    {
      crv3ki = vpcrv13[n0 + ki];
      in3 = crv3ki->in;
      for (kj = 0; kj < idim * in3; kj++)
	crv3ki->ecoef[kj] = -crv3ki->ecoef[kj];
    }


  /* 
   * Spline lofted surface between curve 4 and 2. 
   * --------------------------------------------
   */

  for (ki = 0; ki < n3; ki++)
    switch (ki)
      {
      case 0:
	type[ki] = 1;
	continue;
      case 1:
	type[ki] = 4;
	continue;
      case 2:
	type[ki] = 6;
	continue;
      }
  for (ki = 0; ki < n1; ki++)
    {
      switch (ki)
	{
	case 0:
	  type[ki + n3] = 1;
	  continue;
	case 1:
	  type[ki + n3] = 4;
	  break;
	case 2:
	  type[ki + n3] = 6;
	  break;
	}

      /* 
       * Derivative direction along curve 2
       * must be turned for use in call s1333 
       * ------------------------------------
       */

      if (ki > 0)
	{
	  crv2ki = vpcrv24[ki + n3];
	  in2 = crv2ki->in;
	  for (kj = 0; kj < idim * in2; kj++)
	    crv2ki->ecoef[kj] = -crv2ki->ecoef[kj];
	}
    }

  /* 
   * LOFTING in 2. par. dir. using curve 2 and 4
   * -------------------------------------------
   */

  s1333 (inbcrv24, vpcrv24, type, stpar, iopen, ord13, 0, 
	 &surf24, &cpar, &kstat);
  if (kstat < 0) goto error;

  /* 
   * DEGREE raising if  necessary, first check that
   * order in 1. par. direction is right.
   * ----------------------------------------------
   */

  order2 = MAX( surf24->ik2, ord13 );

  if (surf24->ik1 != ord24) goto err160;

  if (order2 > surf24->ik2)
    s1387 (surf24, ord24, order2, &nsurf24, &kstat);
  else
    nsurf24 = surf24;
  if (kstat < 0) goto error;


  /* 
   * Derivative direction along curve 2
   * must be turned back to original direction 
   * ------------------------------------------
   */

  for (ki = 1; ki < n1; ki++)
    {
      crv2ki = vpcrv24[ki + n3];
      in2 = crv2ki->in;
      for (kj = 0; kj < idim * in2; kj++)
	crv2ki->ecoef[kj] = -crv2ki->ecoef[kj];
    }

  /* 
   * Normalize knot vectors 
   * ----------------------
   */

  s1934 (nsurf13->et1, nsurf13->in1, nsurf13->ik1, start, stop, &kstat);
  if (kstat < 0) goto error;
  s1934 (nsurf13->et2, nsurf13->in2, nsurf13->ik2, start, stop, &kstat);
  if (kstat < 0) goto error;
  s1934 (nsurf24->et1, nsurf24->in1, nsurf24->ik1, start, stop, &kstat);
  if (kstat < 0) goto error;
  s1934 (nsurf24->et2, nsurf24->in2, nsurf24->ik2, start, stop, &kstat);
  if (kstat < 0) goto error;


  /* 
   * Next we find union of knot-vectors nsurf13->et1 and nsurf24->et2,
   * and union of knot-vectors nsurf13->et2 and nsurf24->et1.
   * -----------------------------------------------------------------
   */

  s1935 (nsurf13->et1, nsurf13->in1, nsurf24->et2, nsurf24->in2,
	 &tr1, &nr1, ord13, &kstat);
  if (kstat < 0)
    goto error;

  s1935 (nsurf13->et2, nsurf13->in2, nsurf24->et1, nsurf24->in1,
	 &tr2, &nr2, ord24, &kstat);
  if (kstat < 0)
    goto error;


  /* 
   * Represent the two surfaces with common refined knot-vectors
   * ------------------------------------------------------------
   */

  s1938 (nsurf13, tr1, nr1, tr2, nr2, &srfr1, &kstat);
  if (kstat < 0)
    goto error;

  s1938 (nsurf24, tr2, nr2, tr1, nr1, &srfr2, &kstat);
  if (kstat < 0)
    goto error;


  /* 
   * Allocate array scoef 
   * -------------------
   */

  scoef = newarray (nr1 * nr2 * idim, DOUBLE);
  if (scoef == NULL)
    goto err101;


  /* 
   * Match vertices from surface 1 and 2 
   * ------------------------------------
   */

  s1924 (n0, n1, n2, n3, nr1, nr2, &ew, &kstat);
  if (kstat < 0)
    goto error;

  for (kij = kj = 0; kj < nr2; kj++)
    {
      kjdim = kj * idim;
      for (ki = 0; ki < nr1; ki++, kij++)
	{
	  kidim = ki * idim;
	  sf1ind = kidim * nr2 + kjdim;
	  sf2ind = kjdim * nr1 + kidim;
	  ewrem = ew[kij];

	  for (kk = 0; kk < idim; kk++)
	    scoef[sf2ind + kk] = ewrem * srfr1[sf2ind + kk] +
	      (1 - ewrem) * srfr2[sf1ind + kk];
	}
    }

  /* 
   * Generate the new surface 
   * ------------------------
   */

  *ps1 = newSurf (nr1, nr2, ord13, ord24, tr1, tr2, scoef, 1, idim, 2);
  if (*ps1 == NULL)
    goto err171;

  goto out;


  /* Memory error. */

  err101:
    *jstat = -101;
    s6err ("s1390", *jstat, kpos);
    goto out;

  /* Input error: order of derivative greater than two */

  err151:
    *jstat = -151;
    s6err ("s1390", *jstat, kpos);
    goto out;

  /* Error in orders of generated surfaces */

  err160:
    *jstat = -160;
    s6err ("s1390", *jstat, kpos);
    goto out;

  /* Could not create surface. */

  err171:
    *jstat = -171;
    s6err ("s1390", *jstat, kpos);
    goto out;

  /* Error in lower level routine. */

  error:
    *jstat = kstat;
    s6err ("s1390", *jstat, kpos);
    goto out;

  out:
    if (surf13 != NULL) freeSurf (surf13);
    if (nsurf13 != NULL) freeSurf (nsurf13);
    if (surf24 != NULL) freeSurf (surf24);
    if (nsurf24 != NULL) freeSurf (nsurf24);
    if (ew != NULL) freearray (ew);
    if (srfr1 != NULL) freearray (srfr1);
    if (srfr2 != NULL) freearray (srfr2);
    if (cpar != NULL) freearray (cpar);
    if (rc1 != NULL) freeCurve(rc1);
    if (rc2 != NULL) freeCurve(rc2);

    return;
}
