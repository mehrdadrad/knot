/*  Copyright (C) 2011 CZ.NIC, z.s.p.o. <knot-dns@labs.nic.cz>

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

#include "knot/nameserver/notify.h"

#include "libknot/dname.h"
#include "common/descriptor.h"
#include "libknot/packet/pkt.h"
#include "libknot/rrset.h"
#include "libknot/consts.h"
#include "knot/zone/zonedb.h"
#include "libknot/common.h"
#include "libknot/packet/wire.h"
#include "knot/updates/acl.h"
#include "common/evsched.h"
#include "knot/other/debug.h"
#include "knot/server/server.h"
#include "knot/nameserver/internet.h"
#include "common/debug.h"
#include "knot/nameserver/process_query.h"
#include "knot/nameserver/tsig_ctx.h"
#include "knot/nameserver/process_answer.h"
#include "libknot/dnssec/random.h"
#include "libknot/rrtype/soa.h"

/*----------------------------------------------------------------------------*/
/* API functions                                                              */
/*----------------------------------------------------------------------------*/

/* NOTIFY-specific logging (internal, expects 'qdata' variable set). */
#define NOTIFY_LOG(severity, msg...) \
	QUERY_LOG(severity, qdata, "NOTIFY", msg)

int notify_query(knot_pkt_t *pkt, struct query_data *qdata)
{
	if (pkt == NULL || qdata == NULL) {
		return NS_PROC_FAIL;
	}

	/* RFC1996 require SOA question. */
	zone_t *zone = (zone_t *)qdata->zone;
	NS_NEED_QTYPE(qdata, KNOT_RRTYPE_SOA, KNOT_RCODE_FORMERR);

	/* Check valid zone, transaction security. */
	NS_NEED_ZONE(qdata, KNOT_RCODE_NOTAUTH);
	NS_NEED_AUTH(&zone->conf->acl.notify_in, qdata);

	/* Reserve space for TSIG. */
	knot_pkt_reserve(pkt, tsig_wire_maxsize(qdata->sign.tsig_key));

	/* SOA RR in answer may be included, recover serial. */
	unsigned serial = 0;
	const knot_pktsection_t *answer = knot_pkt_section(qdata->query, KNOT_ANSWER);
	if (answer->count > 0) {
		const knot_rrset_t *soa = &answer->rr[0];
		if (soa->type == KNOT_RRTYPE_SOA) {
			serial = knot_soa_serial(&soa->rrs);
			dbg_ns("%s: received serial %u\n", __func__, serial);
		} else { /* Ignore */
			dbg_ns("%s: NOTIFY answer != SOA_RR\n", __func__);
		}
	}

	/* Incoming NOTIFY expires REFRESH timer and renews EXPIRE timer. */
	zone_events_schedule(zone, ZONE_EVENT_REFRESH, ZONE_EVENT_NOW);

	/* Format resulting log message. */
	NOTIFY_LOG(LOG_INFO, "received serial %u.", serial);
	return NS_PROC_DONE;
}

int notify_process_answer(knot_pkt_t *pkt, struct answer_data *data)
{
	NS_NEED_TSIG_SIGNED(&data->param->tsig_ctx, 0);

	return NS_PROC_DONE; /* No processing. */
}