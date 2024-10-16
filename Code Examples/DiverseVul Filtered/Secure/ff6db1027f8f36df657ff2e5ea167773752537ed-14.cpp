void bgp_capability_send(struct peer *peer, afi_t afi, safi_t safi,
			 int capability_code, int action)
{
	struct stream *s;
	iana_afi_t pkt_afi = IANA_AFI_IPV4;
	iana_safi_t pkt_safi = IANA_SAFI_UNICAST;

	/* Convert AFI, SAFI to values for packet. */
	bgp_map_afi_safi_int2iana(afi, safi, &pkt_afi, &pkt_safi);

	s = stream_new(peer->max_packet_size);

	/* Make BGP update packet. */
	bgp_packet_set_marker(s, BGP_MSG_CAPABILITY);

	/* Encode MP_EXT capability. */
	if (capability_code == CAPABILITY_CODE_MP) {
		stream_putc(s, action);
		stream_putc(s, CAPABILITY_CODE_MP);
		stream_putc(s, CAPABILITY_CODE_MP_LEN);
		stream_putw(s, pkt_afi);
		stream_putc(s, 0);
		stream_putc(s, pkt_safi);

		if (bgp_debug_neighbor_events(peer))
			zlog_debug(
				"%pBP sending CAPABILITY has %s MP_EXT CAP for afi/safi: %s/%s",
				peer,
				action == CAPABILITY_ACTION_SET ? "Advertising"
								: "Removing",
				iana_afi2str(pkt_afi), iana_safi2str(pkt_safi));
	}

	/* Set packet size. */
	bgp_packet_set_size(s);

	/* Add packet to the peer. */
	bgp_packet_add(peer, s);

	bgp_writes_on(peer);
}