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

    Copyright (C) 2015 Kushagra Singh

******************************************************************************/

#include <gmp.h>
#include "flint.h"
#include "fmpz.h"
#include "fmpz_vec.h"
#include "mpn_extras.h"
#include "ulong_extras.h"

void
fmpz_factor_smooth(fmpz_t fac, flint_rand_t state, const fmpz_t n)
{
    mp_limb_t bits, trial_div_cut, currprime, max_iters;
    int i, ret;

    if (fmpz_is_even(n) == 1)
    {
        fmpz_set_ui(fac, UWORD(2));
        return;
    }

    bits = fmpz_bits(n);
    trial_div_cut = 200;

    currprime = UWORD(3);        /* first prime to start trial division with */


    /* TRIAL DIVISION */

    while (currprime < trial_div_cut)
    {
        if (fmpz_fdiv_ui(n, currprime) == 0)
        {
            fmpz_set_ui(fac, currprime);
            return;
        }

        currprime = n_nextprime(currprime, 0);
    }

    max_iters = UWORD(256);

    for (i = 0; i < 3; i++)
    {
        ret = fmpz_factor_pollard_brent(fac, state, n, 2, max_iters)

        if (ret > 0)
            return;
        else
            max_iters <<= UWORD(1);
    }

    /* ECM */

    for (i = 0; i < 6; i++)
    {
        ret = fmpz_factor_ecm(fac, 25, ecm_parameters[i][0], ecm_parameters[i][1], state, n);

        if (ret > 0)
            return;
    }

    /* Known ECM ecm_parameters upto 15 bit factors failed. Going blind */

}