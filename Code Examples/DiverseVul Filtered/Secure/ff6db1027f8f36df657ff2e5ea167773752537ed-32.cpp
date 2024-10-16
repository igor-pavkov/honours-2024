static void bgp_write_proceed_actions(struct peer *peer)
{
	afi_t afi;
	safi_t safi;
	struct peer_af *paf;
	struct bpacket *next_pkt;
	struct update_subgroup *subgrp;
	enum bgp_af_index index;

	for (index = BGP_AF_START; index < BGP_AF_MAX; index++) {
		paf = peer->peer_af_array[index];
		if (!paf)
			continue;

		subgrp = paf->subgroup;
		if (!subgrp)
			continue;

		next_pkt = paf->next_pkt_to_send;
		if (next_pkt && next_pkt->buffer) {
			BGP_TIMER_ON(peer->t_generate_updgrp_packets,
				     bgp_generate_updgrp_packets, 0);
			return;
		}

		/* No packets readily available for AFI/SAFI, are there
		 * subgroup packets
		 * that need to be generated? */
		if (bpacket_queue_is_full(SUBGRP_INST(subgrp),
					  SUBGRP_PKTQ(subgrp))
		    || subgroup_packets_to_build(subgrp)) {
			BGP_TIMER_ON(peer->t_generate_updgrp_packets,
				     bgp_generate_updgrp_packets, 0);
			return;
		}

		afi = paf->afi;
		safi = paf->safi;

		/* No packets to send, see if EOR is pending */
		if (CHECK_FLAG(peer->cap, PEER_CAP_RESTART_RCV)) {
			if (!subgrp->t_coalesce && peer->afc_nego[afi][safi]
			    && peer->synctime
			    && !CHECK_FLAG(peer->af_sflags[afi][safi],
					   PEER_STATUS_EOR_SEND)
			    && safi != SAFI_MPLS_VPN) {
				BGP_TIMER_ON(peer->t_generate_updgrp_packets,
					     bgp_generate_updgrp_packets, 0);
				return;
			}
		}
	}
}