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

    Copyright (C) 2006, 2011 William Hart

******************************************************************************/

#include <gmp.h>
#include <string.h>
#include "flint.h"
#include "ulong_extras.h"
#include "mpqs.h"

void
mpqs_square_root(fmpz_t X, fmpz_t Y, mpqs_t mpqs_inf, uint64_t * nullrows,
                 slong ncols, slong l, fmpz_t N)
{
    slong position, i, j;
    slong * relation = mpqs_inf->relation;
    prime_t * factor_base = mpqs_inf->factor_base;
    slong * prime_count = mpqs_inf->prime_count;
    slong num_primes = mpqs_inf->num_primes;
    fmpz * Y_arr = mpqs_inf->Y_arr; 
    fmpz_t pow;

    fmpz_init(pow);
      
    memset(prime_count, 0, num_primes*sizeof(slong));
      
    fmpz_one(X);
    fmpz_one(Y);
   
    for (i = 0; i < ncols; i++)
    {
        if (mpqs_get_null_entry(nullrows, i, l)) 
        {
            position = mpqs_inf->matrix[i].orig*2*mpqs_inf->max_factors;

            for (j = 0; j < relation[position]; j++)
            {
                prime_count[relation[position+2*j+1]] +=
                   (relation[position+2*j+2]);
            }

            fmpz_mul(Y, Y, Y_arr + mpqs_inf->matrix[i].orig);
            if (i % 10 == 0) fmpz_mod(Y, Y, N);
        }
    }

    for (i = 0; i < num_primes; i++)
    {
        if (prime_count[i]) 
        {
            fmpz_set_ui(pow, factor_base[i].p);
            fmpz_powm_ui(pow, pow, prime_count[i]/2, N);
            fmpz_mul(X, X, pow);
        } 

        if (i%10 == 0 || i == num_primes - 1) fmpz_mod(X, X, N);
    }

#if QS_DEBUG
    for (i = 0; i < num_primes; i++)
        if ((prime_count[i] %2) != 0) flint_printf("Error %wd, %wd, %wd\n",
                                                   l, i, prime_count[i]);
#endif

    fmpz_clear(pow);
}

