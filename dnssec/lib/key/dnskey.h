#pragma once

#include <gnutls/abstract.h>

#include "binary.h"
#include "key.h"

/*!
 * Encode public key to the format used in DNSKEY RDATA.
 *
 * \param key    Public key to be encoded.
 * \param rdata  Encoded key (allocated).
 *
 * \return Error code, DNSSEC_EOK if successful.
 */
int dnskey_pubkey_to_rdata(gnutls_pubkey_t key, dnssec_binary_t *rdata);

/*!
 * Create public key from the format encoded in DNSKEY RDATA.
 *
 * \param algorithm  DNSSEC algorithm identification.
 * \param rdata      Public key in DNSKEY RDATA format.
 * \param key        Created public key.
 *
 * \return Error code, DNSSEC_EOK if successful.
 */
int dnskey_rdata_to_pubkey(uint8_t algorithm, const dnssec_binary_t *rdata,
			   gnutls_pubkey_t key);
