/*  Copyright (C) 2018 CZ.NIC, z.s.p.o. <knot-dns@labs.nic.cz>

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
#include <stdlib.h>

#include "error.h"
#include "key.h"
#include "key/algorithm.h"
#include "key/dnskey.h"
#include "key/internal.h"
#include "key/privkey.h"
#include "keyid.h"
#include "keystore.h"
#include "keystore/internal.h"
#include "shared.h"

/* -- internal API --------------------------------------------------------- */

int keystore_create(dnssec_keystore_t **store_ptr,
		    const keystore_functions_t *functions,
		    void *ctx_custom_data)
{
	assert(store_ptr);
	assert(functions);

	dnssec_keystore_t *store = calloc(1, sizeof(*store));
	if (!store) {
		return DNSSEC_ENOMEM;
	}

	store->functions = functions;

	int result = functions->ctx_new(&store->ctx, ctx_custom_data);
	if (result != DNSSEC_EOK) {
		free(store);
		return DNSSEC_ENOMEM;
	}

	*store_ptr = store;
	return DNSSEC_EOK;
}

/* -- public API ----------------------------------------------------------- */

_public_
int dnssec_keystore_deinit(dnssec_keystore_t *store)
{
	if (!store) {
		return DNSSEC_EINVAL;
	}

	dnssec_keystore_close(store);
	store->functions->ctx_free(store->ctx);

	free(store);

	return DNSSEC_EOK;
}

_public_
int dnssec_keystore_init(dnssec_keystore_t *store, const char *config)
{
	if (!store) {
		return DNSSEC_EINVAL;
	}

	return store->functions->init(store->ctx, config);
}

_public_
int dnssec_keystore_open(dnssec_keystore_t *store, const char *config)
{
	if (!store) {
		return DNSSEC_EINVAL;
	}

	return store->functions->open(store->ctx, config);
}

_public_
int dnssec_keystore_close(dnssec_keystore_t *store)
{
	if (!store) {
		return DNSSEC_EINVAL;
	}

	return store->functions->close(store->ctx);
}

_public_
int dnssec_keystore_list_keys(dnssec_keystore_t *store, dnssec_list_t **list)
{
	if (!store || !list) {
		return DNSSEC_EINVAL;
	}

	return store->functions->list_keys(store->ctx, list);
}

_public_
int dnssec_keystore_generate_key(dnssec_keystore_t *store,
				 dnssec_key_algorithm_t _algorithm,
				 unsigned bits, char **id_ptr)
{
	if (!store || !_algorithm || !id_ptr) {
		return DNSSEC_EINVAL;
	}

	// prepare parameters

	gnutls_pk_algorithm_t algorithm = algorithm_to_gnutls(_algorithm);
	if (algorithm == GNUTLS_PK_UNKNOWN) {
		return DNSSEC_INVALID_KEY_ALGORITHM;
	}

	if (!dnssec_algorithm_key_size_check(_algorithm, bits)) {
		return DNSSEC_INVALID_KEY_SIZE;
	}

	return store->functions->generate_key(store->ctx, algorithm, bits, id_ptr);
}

_public_
int dnssec_keystore_import(dnssec_keystore_t *store, const dnssec_binary_t *pem,
			   char **id_ptr)
{
	if (!store || !pem || !id_ptr) {
		return DNSSEC_EINVAL;
	}

	return store->functions->import_key(store->ctx, pem, id_ptr);
}

_public_
int dnssec_keystore_remove_key(dnssec_keystore_t *store, const char *key_id)
{
	if (!store || !key_id) {
		return DNSSEC_EINVAL;
	}

	return store->functions->remove_key(store->ctx, key_id);
}

_public_
int dnssec_key_import_keystore(dnssec_key_t *key, dnssec_keystore_t *store,
			       const char *id)
{
	if (!key || !store || !id || dnssec_key_get_algorithm(key) == 0) {
		return DNSSEC_EINVAL;
	}

	if (key->private_key) {
		return DNSSEC_KEY_ALREADY_PRESENT;
	}

	gnutls_privkey_t privkey = NULL;
	int r = store->functions->get_private(store->ctx, id, &privkey);
	if (r != DNSSEC_EOK) {
		return r;
	}

	r = key_set_private_key(key, privkey);
	if (r != DNSSEC_EOK) {
		gnutls_privkey_deinit(privkey);
		return r;
	}

	return DNSSEC_EOK;
}
