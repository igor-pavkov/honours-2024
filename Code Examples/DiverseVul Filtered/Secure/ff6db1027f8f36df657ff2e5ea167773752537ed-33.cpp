bool bgp_has_graceful_restart_notification(struct peer *peer)
{
	return CHECK_FLAG(peer->cap, PEER_CAP_GRACEFUL_RESTART_N_BIT_RCV) &&
	       CHECK_FLAG(peer->cap, PEER_CAP_GRACEFUL_RESTART_N_BIT_ADV);
}