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
 * $Id: s1510.c,v 1.1 1994-04-21 12:10:42 boh Exp $
 *
 */


#define S1510

#include "sislP.h"

#if defined(SISLNEEDPROTOTYPES)
void s1510(SISLSurf * ps, double eyepoint[], int idim, double aepsco, 
	   double aepsge, int *jpt, double **gpar, int *jcrv, 
	   SISLIntcurve *** wcurve, int *jstat)
#else
void s1510(ps,eyepoint,idim, aepsco, aepsge, jpt, gpar, jcrv, wcurve, jstat)
     SISLSurf *ps;
     double eyepoint[];
     int idim;
     double aepsco;
     double aepsge;
     int *jpt;
     double **gpar;
     int *jcrv;
     SISLIntcurve ***wcurve;
     int *jstat;
#endif
/*
*********************************************************************
*
*********************************************************************
*
* PURPOSE    : Find the silhouette curves and points of a surface when
*              the surface is viewed perspectively from a specific eye point. 
*              In addition to the points and curves found by this routine,
*              break curves and edge-curves might be silhouette curves.
*
*
*
* INPUT      : ps  -       Pointer to the surface.
*              eyepoint -  The eye point vector.
*              idim   -    Dimension of the space in which eyepoint lies.
*              aepsco -    Computational resolution.
*              aepsge -    Geometry resolution.
*
*
*
* OUTPUT     : jpt    - Number of single silhouette points.
*              gpar   - Array containing the parameter values of the
*                       single silhouette points in the parameter
*                       plane of the surface. The points lie continuous.
*                       Silhouette curves are stored in wcurve.
*              jcrv   - Number of silhouette curves.
*              wcurve - Array containing descriptions of the silhouette
*                       curves. The curves are only described by points
*                       in the parameter plane. The curve-pointers points
*                       to nothing. (See description of Intcurve
*                       in intcurve.dcl).
*              jstat  - status messages
*                                         > 0      : warning
*                                         = 0      : ok
*                                         < 0      : error
*
*
* METHOD     :
*
* REFERENCES : Main routine written by Mike Floater, SI, 1990.
*
* CALLS      : sh1510, s6err.
*
* WRITTEN BY : Christophe Rene Birkeland, SINTEF, 93-06.
*
*********************************************************************
*/
{
  int kstat = 0;           /* Local status variable.                       */
  int kpos = 0;            /* Position of error.                           */
  int i;
  int trackflag = 0;
  int jtrack;
  int *pretop=NULL;
  SISLTrack **wtrack=NULL;
  int jsurf;
  SISLIntsurf **wsurf=NULL;

  sh1510(ps, eyepoint, idim, aepsco, aepsge,trackflag,&jtrack,&wtrack,
	 jpt,gpar,&pretop,jcrv,wcurve,&jsurf,&wsurf,&kstat);
  if(kstat < 0) goto error;

  if(pretop != NULL) freearray(pretop);

  for(i=0; i<jsurf; i++)
    freeIntsurf(wsurf[i]);
  if(wsurf != NULL) freearray(wsurf);

  if(jsurf > 0) 
    *jstat=10;
  else 
    *jstat = kstat;
  goto out;

  /* Error in lower level routine.  */

  error:
    *jstat = kstat;
    s6err ("s1510", *jstat, kpos);
    goto out;

  out:
    return;
}
