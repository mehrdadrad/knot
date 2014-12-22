/*  Copyright (C) 2014 CZ.NIC, z.s.p.o. <knot-dns@labs.nic.cz>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>

#include "error.h"
#include "json.h"
#include "kasp.h"
#include "policy.h"

int load_policy_config(dnssec_kasp_policy_t *policy, const char *filename)
{
	assert(policy);
	assert(filename);

	return DNSSEC_NOT_IMPLEMENTED_ERROR;
}

int save_policy_config(dnssec_kasp_policy_t *policy, const char *filename)
{
	assert(policy);
	assert(filename);

	return DNSSEC_NOT_IMPLEMENTED_ERROR;
}