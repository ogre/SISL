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
 * $Id: s6idnpt.c,v 1.1 1994-04-21 12:10:42 boh Exp $
 *
 */


#define S6IDNPT

#include "sislP.h"

#if defined(SISLNEEDPROTOTYPES)
void 
s6idnpt(SISLIntdat **pintdat,SISLIntpt **pintpt,int itest,int *jstat)
#else
void s6idnpt(pintdat,pintpt,itest,jstat)
     SISLIntdat **pintdat;
     SISLIntpt  **pintpt;
     int    itest;
     int    *jstat;
#endif
/*
*********************************************************************
*
*********************************************************************
*                                                                   
* PURPOSE    : To insert a new intersection point into pintdat.
*              If pintdat is NULL a new pintdat is also made.
*              If pintpt is close to an other intersection point
*              the object pintpt is pointing to is freed, and
*              pintpt is set to point to the already inserted point.
*
*
*
* INPUT      : pintpt   - Pointer to a pointer to new intersection point.
*              pintdat  - Pointer to a pointer to intersection data.
*              itest    - Indikate testing equalety.
*                               = 1      : Testing.
*                               = 0      : No testing.
*
*
* OUTPUT     : jstat  - status messages  
*                               = 2      : Already existing.
*                               = 1      : Already inserted.
*                               = 0      : Intersection point inserted.
*                               < 0      : error
*
*
* METHOD     : 
*
*
* REFERENCES :
*
*-
* CALLS      : s6err      - Gives error message.
*              newIntdat  - Create new intdat structure.
*              freeIntpt  - free instant of intpt structure.
*
* WRITTEN BY : Arne Laksaa, 05.89.
*
*********************************************************************
*/                                     
{
  register int ki,kj;              /* Counters.    */
  
  /* We have to be sure that we have an intdat structure. */
  
  if ((*pintdat) == NULL)
    {
      if (((*pintdat) = newIntdat()) == NULL) goto err101;
    }
  
  
  /* Than we have to be sure that we do not have the intersection point
     before or an equal point. */
  
  for (ki=0; ki<(*pintdat)->ipoint; ki++)
    if ((*pintdat)->vpoint[ki] == (*pintpt))
      {
	*jstat = 1;
	goto out;
      }
    else if (itest && (*pintpt)->iinter != 2)
      {
	for (kj=0; kj<(*pintpt)->ipar; kj++)
	  if (DNEQUAL((*pintpt)->epar[kj],(*pintdat)->vpoint[ki]->epar[kj]))
	    break;
	
	if (kj == (*pintpt)->ipar)
	  {
	    freeIntpt(*pintpt);
	    (*pintpt) = (*pintdat)->vpoint[ki];
	    *jstat = 2;
	    goto out;
	  }
      }
  
  
  /* Than we have to be sure that the array vpoint is great enought. */
  
  if (ki == (*pintdat)->ipmax)
    {
      (*pintdat)->ipmax += 20;
      
      if (((*pintdat)->vpoint = increasearray((*pintdat)->vpoint,
					      (*pintdat)->ipmax,SISLIntpt *)) == NULL) 
	goto err101;
    }
  
  
  /* Now we can insert the new point. */
  
  (*pintdat)->vpoint[ki] = (*pintpt);
  (*pintdat)->ipoint++;
  *jstat = 0;
  goto out;
  

/* Error in space allocation.  */

err101: *jstat = -101;
        s6err("s6idnpt",*jstat,0);
        goto out;

 out: ;
}
