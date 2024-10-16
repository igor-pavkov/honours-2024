static void bgp_refresh_stalepath_timer_expire(struct thread *thread)
{
	struct peer_af *paf;

	paf = THREAD_ARG(thread);

	afi_t afi = paf->afi;
	safi_t safi = paf->safi;
	struct peer *peer = paf->peer;

	peer->t_refresh_stalepath = NULL;

	if (peer->nsf[afi][safi])
		bgp_clear_stale_route(peer, afi, safi);

	if (bgp_debug_neighbor_events(peer))
		zlog_debug(
			"%pBP route-refresh (BoRR) timer expired for afi/safi: %d/%d",
			peer, afi, safi);

	bgp_timer_set(peer);
}