void bgp_check_update_delay(struct bgp *bgp)
{
	struct listnode *node, *nnode;
	struct peer *peer = NULL;

	if (bgp_debug_neighbor_events(peer))
		zlog_debug("Checking update delay, T: %d R: %d I:%d E: %d",
			   bgp->established, bgp->restarted_peers,
			   bgp->implicit_eors, bgp->explicit_eors);

	if (bgp->established
	    <= bgp->restarted_peers + bgp->implicit_eors + bgp->explicit_eors) {
		/*
		 * This is an extra sanity check to make sure we wait for all
		 * the eligible configured peers. This check is performed if
		 * establish wait timer is on, or establish wait option is not
		 * given with the update-delay command
		 */
		if (bgp->t_establish_wait
		    || (bgp->v_establish_wait == bgp->v_update_delay))
			for (ALL_LIST_ELEMENTS(bgp->peer, node, nnode, peer)) {
				if (CHECK_FLAG(peer->flags,
					       PEER_FLAG_CONFIG_NODE)
				    && !CHECK_FLAG(peer->flags,
						   PEER_FLAG_SHUTDOWN)
				    && !CHECK_FLAG(peer->bgp->flags,
						   BGP_FLAG_SHUTDOWN)
				    && !peer->update_delay_over) {
					if (bgp_debug_neighbor_events(peer))
						zlog_debug(
							" Peer %s pending, continuing read-only mode",
							peer->host);
					return;
				}
			}

		zlog_info(
			"Update delay ended, restarted: %d, EORs implicit: %d, explicit: %d",
			bgp->restarted_peers, bgp->implicit_eors,
			bgp->explicit_eors);
		bgp_update_delay_end(bgp);
	}
}