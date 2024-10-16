int bgp_capability_receive(struct peer *peer, bgp_size_t size)
{
	uint8_t *pnt;

	/* Fetch pointer. */
	pnt = stream_pnt(peer->curr);

	if (bgp_debug_neighbor_events(peer))
		zlog_debug("%s rcv CAPABILITY", peer->host);

	/* If peer does not have the capability, send notification. */
	if (!CHECK_FLAG(peer->cap, PEER_CAP_DYNAMIC_ADV)) {
		flog_err(EC_BGP_NO_CAP,
			 "%s [Error] BGP dynamic capability is not enabled",
			 peer->host);
		bgp_notify_send(peer, BGP_NOTIFY_HEADER_ERR,
				BGP_NOTIFY_HEADER_BAD_MESTYPE);
		return BGP_Stop;
	}

	/* Status must be Established. */
	if (!peer_established(peer)) {
		flog_err(
			EC_BGP_NO_CAP,
			"%s [Error] Dynamic capability packet received under status %s",
			peer->host,
			lookup_msg(bgp_status_msg, peer->status, NULL));
		bgp_notify_send(peer, BGP_NOTIFY_FSM_ERR,
				bgp_fsm_error_subcode(peer->status));
		return BGP_Stop;
	}

	/* Parse packet. */
	return bgp_capability_msg_parse(peer, pnt, size);
}