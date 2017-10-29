/*  Copyright (C) 2017 CZ.NIC, z.s.p.o. <knot-dns@labs.nic.cz>

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
/*!
 * \file
 *
 * Server configuration interface.
 *
 * \addtogroup config
 *
 * @{
 */

#pragma once

#include <sys/socket.h>

#include "knot/conf/base.h"
#include "knot/conf/schema.h"

#define CONF_XFERS	10

/*! Configuration remote getter output. */
typedef struct {
	/*! Target socket address. */
	struct sockaddr_storage addr;
	/*! Local outgoing socket address. */
	struct sockaddr_storage via;
	/*! TSIG key. */
	knot_tsig_key_t key;
} conf_remote_t;

/*! Configuration section iterator. */
typedef struct {
	/*! Item description. */
	const yp_item_t *item;
	/*! Namedb iterator. */
	knot_db_iter_t *iter;
	/*! Key0 database code. */
	uint8_t key0_code;
	// Public items.
	/*! Iterator return code. */
	int code;
} conf_iter_t;

/*! Configuration module getter output. */
typedef struct {
	/*! Module name. */
	yp_name_t *name;
	/*! Module id data. */
	uint8_t *data;
	/*! Module id data length. */
	size_t len;
} conf_mod_id_t;

/*!
 * Gets the configuration item value of the section without identifiers.
 *
 * \param[in] conf       Configuration.
 * \param[in] txn        Configuration DB transaction.
 * \param[in] key0_name  Section name.
 * \param[in] key1_name  Item name.
 *
 * \return Item value.
 */
conf_val_t conf_get_txn(
	conf_t *conf,
	knot_db_txn_t *txn,
	const yp_name_t *key0_name,
	const yp_name_t *key1_name
);
static inline conf_val_t conf_get(
	conf_t *conf,
	const yp_name_t *key0_name,
	const yp_name_t *key1_name)
{
	return conf_get_txn(conf, &conf->read_txn, key0_name, key1_name);
}

/*!
 * Gets the configuration item value of the section with identifiers (raw version).
 *
 * \param[in] conf        Configuration.
 * \param[in] txn         Configuration DB transaction.
 * \param[in] key0_name   Section name.
 * \param[in] key1_name   Item name.
 * \param[in] id          Section identifier (raw value).
 * \param[in] id_len      Length of the section identifier.
 *
 * \return Item value.
 */
conf_val_t conf_rawid_get_txn(
	conf_t *conf,
	knot_db_txn_t *txn,
	const yp_name_t *key0_name,
	const yp_name_t *key1_name,
	const uint8_t *id,
	size_t id_len
);
static inline conf_val_t conf_rawid_get(
	conf_t *conf,
	const yp_name_t *key0_name,
	const yp_name_t *key1_name,
	const uint8_t *id,
	size_t id_len)
{
	return conf_rawid_get_txn(conf, &conf->read_txn, key0_name, key1_name,
	                          id, id_len);
}

/*!
 * Gets the configuration item value of the section with identifiers.
 *
 * \param[in] conf        Configuration.
 * \param[in] txn         Configuration DB transaction.
 * \param[in] key0_name   Section name.
 * \param[in] key1_name   Item name.
 * \param[in] id          Section identifier (output of a config getter).
 *
 * \return Item value.
 */
conf_val_t conf_id_get_txn(
	conf_t *conf,
	knot_db_txn_t *txn,
	const yp_name_t *key0_name,
	const yp_name_t *key1_name,
	conf_val_t *id
);
static inline conf_val_t conf_id_get(
	conf_t *conf,
	const yp_name_t *key0_name,
	const yp_name_t *key1_name,
	conf_val_t *id)
{
	return conf_id_get_txn(conf, &conf->read_txn, key0_name, key1_name, id);
}

/*!
 * Gets the configuration item value of the module section.
 *
 * \param[in] conf       Configuration.
 * \param[in] txn        Configuration DB transaction.
 * \param[in] key1_name  Item name.
 * \param[in] mod_id     Module identifier.
 *
 * \return Item value.
 */
conf_val_t conf_mod_get_txn(
	conf_t *conf,
	knot_db_txn_t *txn,
	const yp_name_t *key1_name,
	const conf_mod_id_t *mod_id
);
static inline conf_val_t conf_mod_get(
	conf_t *conf,
	const yp_name_t *key1_name,
	const conf_mod_id_t *mod_id)
{
	return conf_mod_get_txn(conf, &conf->read_txn, key1_name, mod_id);
}

/*!
 * Gets the configuration item value of the zone section.
 *
 * \note A possibly associated template is taken into account.
 *
 * \param[in] conf        Configuration.
 * \param[in] txn         Configuration DB transaction.
 * \param[in] key1_name   Item name.
 * \param[in] dname Zone  name.
 *
 * \return Item value.
 */
conf_val_t conf_zone_get_txn(
	conf_t *conf,
	knot_db_txn_t *txn,
	const yp_name_t *key1_name,
	const knot_dname_t *dname
);
static inline conf_val_t conf_zone_get(
	conf_t *conf,
	const yp_name_t *key1_name,
	const knot_dname_t *dname)
{
	return conf_zone_get_txn(conf, &conf->read_txn, key1_name, dname);
}

/*!
 * Gets the configuration item value of the default template.
 *
 * \param[in] conf       Configuration.
 * \param[in] txn        Configuration DB transaction.
 * \param[in] key1_name  Item name.
 *
 * \return Item value.
 */
conf_val_t conf_default_get_txn(
	conf_t *conf,
	knot_db_txn_t *txn,
	const yp_name_t *key1_name
);
static inline conf_val_t conf_default_get(
	conf_t *conf,
	const yp_name_t *key1_name)
{
	return conf_default_get_txn(conf, &conf->read_txn, key1_name);
}

/*!
 * Checks the configuration section for the identifier (raw version).
 *
 * \param[in] conf        Configuration.
 * \param[in] txn         Configuration DB transaction.
 * \param[in] key0_name   Section name.
 * \param[in] id          Section identifier (raw value).
 * \param[in] id_len      Length of the section identifier.
 *
 * \return True if exists.
 */
bool conf_rawid_exists_txn(
	conf_t *conf,
	knot_db_txn_t *txn,
	const yp_name_t *key0_name,
	const uint8_t *id,
	size_t id_len
);
static inline bool conf_rawid_exists(
	conf_t *conf,
	const yp_name_t *key0_name,
	const uint8_t *id,
	size_t id_len)
{
	return conf_rawid_exists_txn(conf, &conf->read_txn, key0_name, id, id_len);
}

/*!
 * Checks the configuration section for the identifier.
 *
 * \param[in] conf        Configuration.
 * \param[in] txn         Configuration DB transaction.
 * \param[in] key0_name   Section name.
 * \param[in] id          Section identifier (output of a config getter).
 *
 * \return True if exists.
 */
bool conf_id_exists_txn(
	conf_t *conf,
	knot_db_txn_t *txn,
	const yp_name_t *key0_name,
	conf_val_t *id
);
static inline bool conf_id_exists(
	conf_t *conf,
	const yp_name_t *key0_name,
	conf_val_t *id)
{
	return conf_id_exists_txn(conf, &conf->read_txn, key0_name, id);
}

/*!
 * Gets the number of section identifiers.
 *
 * \param[in] conf       Configuration.
 * \param[in] txn        Configuration DB transaction.
 * \param[in] key0_name  Section name.
 *
 * \return Number of identifiers.
 */
size_t conf_id_count_txn(
	conf_t *conf,
	knot_db_txn_t *txn,
	const yp_name_t *key0_name
);
static inline size_t conf_id_count(
	conf_t *conf,
	const yp_name_t *key0_name)
{
	return conf_id_count_txn(conf, &conf->read_txn, key0_name);
}

/*!
 * Gets a configuration section iterator.
 *
 * \param[in] conf       Configuration.
 * \param[in] txn        Configuration DB transaction.
 * \param[in] key0_name  Section name.
 *
 * \return Section iterator.
 */
conf_iter_t conf_iter_txn(
	conf_t *conf,
	knot_db_txn_t *txn,
	const yp_name_t *key0_name
);
static inline conf_iter_t conf_iter(
	conf_t *conf,
	const yp_name_t *key0_name)
{
	return conf_iter_txn(conf, &conf->read_txn, key0_name);
}

/*!
 * Moves the configuration section iterator to the next identifier.
 *
 * \param[in] conf  Configuration.
 * \param[in] iter  Configuration iterator.
 */
void conf_iter_next(
	conf_t *conf,
	conf_iter_t *iter
);

/*!
 * Gets the current iterator value (identifier).
 *
 * \param[in] conf  Configuration.
 * \param[in] iter  Configuration iterator.
 *
 * \return Section identifier.
 */
conf_val_t conf_iter_id(
	conf_t *conf,
	conf_iter_t *iter
);

/*!
 * Deletes the section iterator.
 *
 * This function should be called when the iterating is early interrupted,
 * otherwise this is done automaticaly at KNOT_EOF.
 *
 * \param[in] conf  Configuration.
 * \param[in] iter  Configuration iterator.
 */
void conf_iter_finish(
	conf_t *conf,
	conf_iter_t *iter
);

/*!
 * Prepares the value for the direct access.
 *
 * The following access is through val->len and val->data.
 *
 * \param[in] val  Item value.
 */
void conf_val(
	conf_val_t *val
);

/*!
 * Moves to the next item value.
 *
 * \param[in] val  Item value.
 */
void conf_val_next(
	conf_val_t *val
);

/*!
 * Gets the number of values if multivalued item.
 *
 * \param[in] val  Item value.
 *
 * \return Number of values.
 */
size_t conf_val_count(
	conf_val_t *val
);

/*!
 * Checks if two item values are equal.
 *
 * \param[in] val1  First item value.
 * \param[in] val2  Second item value.
 *
 * \return true if equal, false if not.
 */
bool conf_val_equal(
	conf_val_t *val1,
	conf_val_t *val2
);

/*!
 * Gets the numeric value of the item.
 *
 * \param[in] val  Item value.
 *
 * \return Integer.
 */
int64_t conf_int(
	conf_val_t *val
);

/*!
 * Gets the boolean value of the item.
 *
 * \param[in] val  Item value.
 *
 * \return Boolean.
 */
bool conf_bool(
	conf_val_t *val
);

/*!
 * Gets the option value of the item.
 *
 * \param[in] val  Item value.
 *
 * \return Option id.
 */
unsigned conf_opt(
	conf_val_t *val
);

/*!
 * Gets the string value of the item.
 *
 * \param[in] val  Item value.
 *
 * \return String pointer.
 */
const char* conf_str(
	conf_val_t *val
);

/*!
 * Gets the dname value of the item.
 *
 * \param[in] val  Item value.
 *
 * \return Dname pointer.
 */
const knot_dname_t* conf_dname(
	conf_val_t *val
);

/*!
 * Gets the length-prefixed data value of the item.
 *
 * \param[in] val   Item value.
 * \param[out] len  Output length.
 *
 * \return Data pointer.
 */
const uint8_t* conf_bin(
	conf_val_t *val,
	size_t *len
);

/*!
 * Gets the generic data value of the item.
 *
 * \param[in] val   Item value.
 * \param[out] len  Output length.
 *
 * \return Data pointer.
 */
const uint8_t* conf_data(
	conf_val_t *val,
	size_t *len
);

/*!
 * Gets the socket address value of the item.
 *
 * \param[in] val            Item value.
 * \param[in] sock_base_dir  Path prefix for a relative UNIX socket location.
 *
 * \return Socket address.
 */
struct sockaddr_storage conf_addr(
	conf_val_t *val,
	const char *sock_base_dir
);

/*!
 * Gets the socket address range value of the item.
 *
 * \param[in] val            Item value.
 * \param[out] max_ss Upper  address bound or AF_UNSPEC family if not specified.
 * \param[out] prefix_len    Network subnet prefix length or -1 if not specified.
 *
 * \return Socket address.
 */
struct sockaddr_storage conf_addr_range(
	conf_val_t *val,
	struct sockaddr_storage *max_ss,
	int *prefix_len
);

/*!
 * Checks the address if matches given address range/network block.
 *
 * \param[in] range  Address range/network block.
 * \param[in] addr   Address to check.
 *
 * \return True if matches.
 */
bool conf_addr_range_match(
	conf_val_t *range,
	const struct sockaddr_storage *addr
);

/*!
 * Gets the absolute string value of the item.
 *
 * \note The result must be explicitly deallocated.
 *
 * \param[in] val       Item value.
 * \param[in] base_dir  Path prefix for a relative string.
 *
 * \return Absolute path string pointer.
 */
char* conf_abs_path(
	conf_val_t *val,
	const char *base_dir
);

/*!
 * Ensures empty 'default' identifier value.
 *
 * \param[in] val  Item value.
 *
 * \return Empty item value.
 */
static inline void conf_id_fix_default(conf_val_t *val)
{
	if (val->code != KNOT_EOK) {
		conf_val_t empty = {
			.item = val->item,
			.code = KNOT_EOK
		};

		*val = empty;
	}
}

/*!
 * Gets the module identifier value of the item.
 *
 * \param[in] val  Item value.
 *
 * \return Module identifier.
 */
conf_mod_id_t* conf_mod_id(
	conf_val_t *val
);

/*!
 * Destroys the module identifier.
 *
 * \param[in] mod_id  Module identifier.
 */
void conf_free_mod_id(
	conf_mod_id_t *mod_id
);

/*!
 * Gets the absolute zone file path.
 *
 * \note The result must be explicitly deallocated.
 *
 * \param[in] conf  Configuration.
 * \param[in] txn   Configuration DB transaction.
 * \param[in] zone  Zone name.
 *
 * \return Absolute zonef ile path string pointer.
 */
char* conf_zonefile_txn(
	conf_t *conf,
	knot_db_txn_t *txn,
	const knot_dname_t *zone
);
static inline char* conf_zonefile(
	conf_t *conf,
	const knot_dname_t *zone)
{
	return conf_zonefile_txn(conf, &conf->read_txn, zone);
}

/*!
 * Gets the way zonefile is loaded.
 *
 * \param[in] conf  Configuration.
 * \param[in] txn   Configuration DB transaction.
 * \param[in] zone  Zone name.
 *
 * \return ZONEFILE_LOAD_*
 */
unsigned conf_zonefile_load_txn(
	conf_t *conf,
	knot_db_txn_t *txn,
	const knot_dname_t *zone
);

static inline unsigned conf_zonefile_load(
	conf_t *conf,
	const knot_dname_t *zone)
{
	return conf_zonefile_load_txn(conf, &conf->read_txn, zone);
}

/*!
 * Gets the absolute journal file path.
 *
 * \note The result must be explicitly deallocated.
 *
 * \param[in] conf  Configuration.
 * \param[in] txn   Configuration DB transaction.
 *
 * \return Absolute journal file path string pointer.
 */
char* conf_journalfile_txn(
	conf_t *conf,
	knot_db_txn_t *txn);
static inline char* conf_journalfile(
	conf_t *conf)
{
	return conf_journalfile_txn(conf, &conf->read_txn);
}

char* conf_kaspdir_txn(
	conf_t *conf,
	knot_db_txn_t *txn);
static inline char* conf_kaspdir(
	conf_t *conf)
{
	return conf_kaspdir_txn(conf, &conf->read_txn);
}

/*!
 * Gets the configured number of UDP threads.
 *
 * \param[in] conf  Configuration.
 * \param[in] txn   Configuration DB transaction.
 *
 * \return Number of threads.
 */
size_t conf_udp_threads_txn(
	conf_t *conf,
	knot_db_txn_t *txn
);
static inline size_t conf_udp_threads(
	conf_t *conf)
{
	return conf_udp_threads_txn(conf, &conf->read_txn);
}

/*!
 * Gets the configured number of TCP threads.
 *
 * \param[in] conf  Configuration.
 * \param[in] txn   Configuration DB transaction.
 *
 * \return Number of threads.
 */
size_t conf_tcp_threads_txn(
	conf_t *conf,
	knot_db_txn_t *txn
);
static inline size_t conf_tcp_threads(
	conf_t *conf)
{
	return conf_tcp_threads_txn(conf, &conf->read_txn);
}

/*!
 * Gets the configured number of worker threads.
 *
 * \param[in] conf  Configuration.
 * \param[in] txn   Configuration DB transaction.
 *
 * \return Number of threads.
 */
size_t conf_bg_threads_txn(
	conf_t *conf,
	knot_db_txn_t *txn
);
static inline size_t conf_bg_threads(
	conf_t *conf)
{
	return conf_bg_threads_txn(conf, &conf->read_txn);
}

/*!
 * Gets the configured user and group identifiers.
 *
 * \param[in] conf  Configuration.
 * \param[in] txn   Configuration DB transaction.
 * \param[out] uid  User identifier.
 * \param[out] gid  Group identifier.
 *
 * \return Knot error code.
 */
int conf_user_txn(
	conf_t *conf,
	knot_db_txn_t *txn,
	int *uid,
	int *gid
);
static inline int conf_user(
	conf_t *conf,
	int *uid,
	int *gid)
{
	return conf_user_txn(conf, &conf->read_txn, uid, gid);
}

/*!
 * Gets the remote parameters for the given identifier.
 *
 * \param[in] conf   Configuration.
 * \param[in] txn    Configuration DB transaction.
 * \param[in] id     Remote identifier.
 * \param[in] index  Remote index (counted from 0).
 *
 * \return Remote parameters.
 */
conf_remote_t conf_remote_txn(
	conf_t *conf,
	knot_db_txn_t *txn,
	conf_val_t *id,
	size_t index
);
static inline conf_remote_t conf_remote(
	conf_t *conf,
	conf_val_t *id,
	size_t index)
{
	return conf_remote_txn(conf, &conf->read_txn, id, index);

}

/*! @} */
