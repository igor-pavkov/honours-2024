static int bgp_keepalive_receive(struct peer *peer, bgp_size_t size)
{
	if (bgp_debug_keepalive(peer))
		zlog_debug("%s KEEPALIVE rcvd", peer->host);

	bgp_update_implicit_eors(peer);

	peer->rtt = sockopt_tcp_rtt(peer->fd);

	/* If the peer's RTT is higher than expected, shutdown
	 * the peer automatically.
	 */
	if (CHECK_FLAG(peer->flags, PEER_FLAG_RTT_SHUTDOWN)
	    && peer->rtt > peer->rtt_expected) {

		peer->rtt_keepalive_rcv++;

		if (peer->rtt_keepalive_rcv > peer->rtt_keepalive_conf) {
			zlog_warn(
				"%s shutdown due to high round-trip-time (%dms > %dms)",
				peer->host, peer->rtt, peer->rtt_expected);
			peer_flag_set(peer, PEER_FLAG_SHUTDOWN);
		}
	} else {
		if (peer->rtt_keepalive_rcv)
			peer->rtt_keepalive_rcv--;
	}

	return Receive_KEEPALIVE_message;
}