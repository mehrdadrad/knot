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

#pragma once

#include <gnutls/gnutls.h>
#include <gnutls/abstract.h>

#include "key.h"
#include "keystore.h"

typedef struct keystore_functions {
	// construction of internal context
	int (*ctx_new)(void **ctx_ptr, void *custom_data);
	void (*ctx_free)(void *ctx);
	// keystore open/close
	int (*open)(void *ctx, const char *config);
	int (*close)(void *ctx);
	// keystore access
	int (*list_keys)(void *ctx, void *list);
	int (*generate_key)(void *ctx, gnutls_pk_algorithm_t algorithm,
			    unsigned bits, char **id_ptr);
	int (*delete_key)(void *ctx, const char *id);
	// private key access
	int (*get_private)(void *ctx, const char *id, gnutls_privkey_t *key_ptr);
} keystore_functions_t;

struct dnssec_keystore {
	const keystore_functions_t *functions;
	void *ctx;
};

int keystore_create(dnssec_keystore_t **store_ptr,
		    const keystore_functions_t *functions,
		    void *ctx_custom_data, const char *open_config);