static void bgp_update_explicit_eors(struct peer *peer)
{
	afi_t afi;
	safi_t safi;

	if (!bgp_update_delay_active(peer->bgp))
		return; /* BGP update delay has ended */
	if (peer->update_delay_over)
		return; /* This peer has already been considered */

	if (bgp_debug_neighbor_events(peer))
		zlog_debug("Peer %s: Checking explicit EORs", peer->host);

	FOREACH_AFI_SAFI (afi, safi) {
		if (peer->afc_nego[afi][safi]
		    && !CHECK_FLAG(peer->af_sflags[afi][safi],
				   PEER_STATUS_EOR_RECEIVED)) {
			if (bgp_debug_neighbor_events(peer))
				zlog_debug(
					"   afi %d safi %d didn't receive EOR",
					afi, safi);
			return;
		}
	}

	peer->update_delay_over = 1;
	peer->bgp->explicit_eors++;
	bgp_check_update_delay(peer->bgp);
}