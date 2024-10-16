void bgp_packet_process_error(struct thread *thread)
{
	struct peer *peer;
	int code;

	peer = THREAD_ARG(thread);
	code = THREAD_VAL(thread);

	if (bgp_debug_neighbor_events(peer))
		zlog_debug("%s [Event] BGP error %d on fd %d",
			   peer->host, code, peer->fd);

	/* Closed connection or error on the socket */
	if (peer_established(peer)) {
		if ((CHECK_FLAG(peer->flags, PEER_FLAG_GRACEFUL_RESTART)
		     || CHECK_FLAG(peer->flags,
				   PEER_FLAG_GRACEFUL_RESTART_HELPER))
		    && CHECK_FLAG(peer->sflags, PEER_STATUS_NSF_MODE)) {
			peer->last_reset = PEER_DOWN_NSF_CLOSE_SESSION;
			SET_FLAG(peer->sflags, PEER_STATUS_NSF_WAIT);
		} else
			peer->last_reset = PEER_DOWN_CLOSE_SESSION;
	}

	bgp_event_update(peer, code);
}