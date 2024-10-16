void bgp_open_send(struct peer *peer)
{
	struct stream *s;
	uint16_t send_holdtime;
	as_t local_as;

	if (CHECK_FLAG(peer->flags, PEER_FLAG_TIMER))
		send_holdtime = peer->holdtime;
	else
		send_holdtime = peer->bgp->default_holdtime;

	/* local-as Change */
	if (peer->change_local_as)
		local_as = peer->change_local_as;
	else
		local_as = peer->local_as;

	s = stream_new(BGP_STANDARD_MESSAGE_MAX_PACKET_SIZE);

	/* Make open packet. */
	bgp_packet_set_marker(s, BGP_MSG_OPEN);

	/* Set open packet values. */
	stream_putc(s, BGP_VERSION_4); /* BGP version */
	stream_putw(s, (local_as <= BGP_AS_MAX) ? (uint16_t)local_as
						: BGP_AS_TRANS);
	stream_putw(s, send_holdtime);		/* Hold Time */
	stream_put_in_addr(s, &peer->local_id); /* BGP Identifier */

	/* Set capabilities */
	if (CHECK_FLAG(peer->flags, PEER_FLAG_EXTENDED_OPT_PARAMS)) {
		(void)bgp_open_capability(s, peer, true);
	} else {
		struct stream *tmp = stream_new(STREAM_SIZE(s));

		stream_copy(tmp, s);
		if (bgp_open_capability(tmp, peer, false)
		    > BGP_OPEN_NON_EXT_OPT_LEN) {
			stream_free(tmp);
			(void)bgp_open_capability(s, peer, true);
		} else {
			stream_copy(s, tmp);
			stream_free(tmp);
		}
	}

	/* Set BGP packet length. */
	bgp_packet_set_size(s);

	if (bgp_debug_neighbor_events(peer))
		zlog_debug(
			"%s sending OPEN, version %d, my as %u, holdtime %d, id %pI4",
			peer->host, BGP_VERSION_4, local_as, send_holdtime,
			&peer->local_id);

	/* Dump packet if debug option is set. */
	/* bgp_packet_dump (s); */
	hook_call(bgp_packet_send, peer, BGP_MSG_OPEN, stream_get_endp(s), s);

	/* Add packet to the peer. */
	bgp_packet_add(peer, s);

	bgp_writes_on(peer);
}