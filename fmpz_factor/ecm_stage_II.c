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

/* Implementation of the stage I of ECM */

int
fmpz_factor_ecm_stage_II(mp_ptr f, mp_limb_t B1, mp_limb_t B2, mp_limb_t P,
                          mp_ptr n, ecm_t ecm_inf)
{
    
    mp_ptr Qx, Qz, Rx, Rz, Qdx, Qdz, a, b, g;
    mp_limb_t mmin, mmax, maxj, sz, gcdlimbs;
    int i, j, ret;
    mp_ptr arrx, arrz;

    mmin = (B1 + (P/2)) / P;
    mmax = ((B2 - P/2) + P - 1)/P;      /* ceil */
    maxj = (P + 1)/2; 

    Qx = flint_malloc(ecm_inf->n_size * sizeof(mp_limb_t));
    Qz = flint_malloc(ecm_inf->n_size * sizeof(mp_limb_t));
    Rx = flint_malloc(ecm_inf->n_size * sizeof(mp_limb_t));
    Rz = flint_malloc(ecm_inf->n_size * sizeof(mp_limb_t));
    Qdx = flint_malloc(ecm_inf->n_size * sizeof(mp_limb_t));
    Qdz = flint_malloc(ecm_inf->n_size * sizeof(mp_limb_t));
    a = flint_malloc(ecm_inf->n_size * sizeof(mp_limb_t));
    b = flint_malloc(ecm_inf->n_size * sizeof(mp_limb_t));
    g = flint_malloc(ecm_inf->n_size * sizeof(mp_limb_t));

    g[0] = ecm_inf->one[0];

    arrx = flint_malloc((maxj + 1) * ecm_inf->n_size * sizeof(mp_limb_t));
    arrz = flint_malloc((maxj + 1) * ecm_inf->n_size * sizeof(mp_limb_t));

    mpn_zero(arrx, (maxj + 1) * ecm_inf->n_size);
    mpn_zero(arrz, (maxj + 1) * ecm_inf->n_size);

    ret = 0;

    /* arr[1] = Q0 */
    mpn_copyi(arrx + 1 * ecm_inf->n_size, ecm_inf->x, ecm_inf->n_size);
    mpn_copyi(arrz + 1 * ecm_inf->n_size, ecm_inf->z, ecm_inf->n_size);

    /* arr[2] = 2Q0 */

    fmpz_factor_ecm_double(arrx + 2 * ecm_inf->n_size, arrz + 2 * ecm_inf->n_size,
                            arrx + 1 * ecm_inf->n_size, arrz + 1 * ecm_inf->n_size,
                            n, ecm_inf);

    /* arr[3] = 3Q0 */
    fmpz_factor_ecm_add(arrx + 3 * ecm_inf->n_size, arrz + 3 * ecm_inf->n_size,
                         arrx + 2 * ecm_inf->n_size, arrz + 2 * ecm_inf->n_size,
                         arrx + 1 * ecm_inf->n_size, arrz + 1 * ecm_inf->n_size,
                         arrx + 1 * ecm_inf->n_size, arrz + 1 * ecm_inf->n_size,
                         n, ecm_inf);

    /* For each odd j (j > 3) , compute j * Q0 [x0 :: z0] */

    /* We are adding 2Q0 every time. Need to calculate all j's 
       as (j - 2)Q0 is required for (j + 2)Q0 */

    for (j = 5; j <= maxj; j += 2)
    {
        /* jQ0 = (j - 2)Q0 + 2Q0 
           Differnce is (j - 4)Q0 */

        fmpz_factor_ecm_add(arrx + j * ecm_inf->n_size, arrz + j * ecm_inf->n_size,
                             arrx + (j - 2) * ecm_inf->n_size, arrz + (j - 2) * ecm_inf->n_size,
                             arrx + 2 * ecm_inf->n_size, arrz + 2 * ecm_inf->n_size,
                             arrx + (j - 4) * ecm_inf->n_size, arrz + (j - 4) * ecm_inf->n_size,
                             n, ecm_inf);
    }

    /* Q = P * Q_0 */
    fmpz_factor_ecm_mul_montgomery_ladder(Qx, Qz, ecm_inf->x, ecm_inf->z,
                                           P, n, ecm_inf);
    /* R = mmin * Q */
    fmpz_factor_ecm_mul_montgomery_ladder(Rx, Rz, Qx, Qz, mmin, n, ecm_inf);

    /* Qd = (mmin - 1) * Q */
    fmpz_factor_ecm_mul_montgomery_ladder(Qdx, Qdz, Qx, Qz, mmin - 1, n, ecm_inf);

    /* main stage II step */

    for (i = mmin; i <= mmax; i ++)
    {
        for (j = 1; j <= maxj; j+=2)
        {
            if (ecm_inf->prime_table[i - mmin][j] == 1)
            {
                flint_mpn_mulmod_preinvn(a, Rx, arrz + j * ecm_inf->n_size,
                                         ecm_inf->n_size, n, ecm_inf->ninv,
                                         ecm_inf->normbits);

                flint_mpn_mulmod_preinvn(b, Rz, arrx + j * ecm_inf->n_size,
                                         ecm_inf->n_size, n, ecm_inf->ninv,
                                         ecm_inf->normbits);


                fmpz_factor_ecm_submod(a, a, b, n, ecm_inf->n_size);

                flint_mpn_mulmod_preinvn(g, g, a, ecm_inf->n_size, n, ecm_inf->ninv,
                                         ecm_inf->normbits);
            }
        }

        mpn_copyi(a, Rx, ecm_inf->n_size);
        mpn_copyi(b, Rz, ecm_inf->n_size);

        /* R = R + Q    
           difference is stored in Qd, initially (Mmin - 1)Q */

        fmpz_factor_ecm_add(Rx, Rz, Rx, Rz, Qx, Qz, Qdx, Qdz, n, ecm_inf);

        mpn_copyi(Qdx, a, ecm_inf->n_size);
        mpn_copyi(Qdz, b, ecm_inf->n_size);
    }

    sz = ecm_inf->n_size;
    MPN_NORM(g, sz);

    if (sz == 0)
    {
        ret = 0;
        goto cleanup;
    }

    gcdlimbs = flint_mpn_gcd_full(f, n, ecm_inf->n_size, g, sz);

    /* condition one -> gcd = n_ecm->one
       condition two -> gcd = n
       if neither is true, factor found */

    if ((((gcdlimbs == 1) && f[0] == ecm_inf->one[0]) || 
        ((gcdlimbs == ecm_inf->n_size) && mpn_cmp(f, n, ecm_inf->n_size) == 0)) == 0)
    {
        /* Found factor in stage I */
        ret = gcdlimbs;
        goto cleanup;
    }

    cleanup:

    flint_free(Qx);
    flint_free(Qz);
    flint_free(Rx);
    flint_free(Rz);
    flint_free(Qdx);
    flint_free(Qdz);
    flint_free(a);
    flint_free(b);
    flint_free(g);

    flint_free(arrx);
    flint_free(arrz);

    return ret;
}