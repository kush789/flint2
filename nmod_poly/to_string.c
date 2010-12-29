/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2010 William Hart

******************************************************************************/

#include <mpir.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "flint.h"
#include "nmod_poly.h"

char * nmod_poly_to_string(nmod_poly_t poly)
{
    ulong i;
    char * buf, * ptr;

    /* estimate for the length, n and three spaces */
#if FLINT64
    ulong size = 21*2 + 1;
#else
    ulong size = 11*2 + 1;
#endif

    for (i = 0; i < poly->length; i++)
    {
        if (poly->coeffs[i]) /* log(2)/log(10) < 0.30103, +1 for space/null */
            size += (ulong) ceil(0.30103*FLINT_BIT_COUNT(poly->coeffs[i])) + 1;
        else size += 2;
    }

    buf = (char *) malloc(size);  
    ptr = buf + sprintf(buf, "%lu %lu ", poly->length, poly->mod.n);
   
    for (i = 0; i < poly->length; i++)
        ptr += sprintf(ptr, " %lu", poly->coeffs[i]);
   
    if (poly->length == 0)
        sprintf(ptr, " ");

    return buf;
}