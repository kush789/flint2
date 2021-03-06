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

	Built upon existing FLINT siqs
    Copyright (C) 2015 Kushagra Singh

******************************************************************************/

#include <gmp.h>
#include "flint.h"
#include "ulong_extras.h"
#include "mpqs.h"

void
mpqs_clear(mpqs_t mpqs_inf)
{
    fmpz_clear(mpqs_inf->n);
    fmpz_clear(mpqs_inf->kn);

    flint_free(mpqs_inf->factor_base);
    flint_free(mpqs_inf->sqrts);

    mpqs_inf->factor_base = NULL;
    mpqs_inf->sqrts       = NULL;
}
