void bgp_update_restarted_peers(struct peer *peer)
{
	if (!bgp_update_delay_active(peer->bgp))
		return; /* BGP update delay has ended */
	if (peer->update_delay_over)
		return; /* This peer has already been considered */

	if (bgp_debug_neighbor_events(peer))
		zlog_debug("Peer %s: Checking restarted", peer->host);

	if (peer_established(peer)) {
		peer->update_delay_over = 1;
		peer->bgp->restarted_peers++;
		bgp_check_update_delay(peer->bgp);
	}
}