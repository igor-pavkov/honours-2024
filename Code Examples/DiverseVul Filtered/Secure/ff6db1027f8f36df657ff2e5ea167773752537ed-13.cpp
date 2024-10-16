void bgp_keepalive_send(struct peer *peer)
{
	struct stream *s;

	s = stream_new(BGP_STANDARD_MESSAGE_MAX_PACKET_SIZE);

	/* Make keepalive packet. */
	bgp_packet_set_marker(s, BGP_MSG_KEEPALIVE);

	/* Set packet size. */
	bgp_packet_set_size(s);

	/* Dump packet if debug option is set. */
	/* bgp_packet_dump (s); */

	if (bgp_debug_keepalive(peer))
		zlog_debug("%s sending KEEPALIVE", peer->host);

	/* Add packet to the peer. */
	bgp_packet_add(peer, s);

	bgp_writes_on(peer);
}