static struct stream *bgp_update_packet_eor(struct peer *peer, afi_t afi,
					    safi_t safi)
{
	struct stream *s;
	iana_afi_t pkt_afi = IANA_AFI_IPV4;
	iana_safi_t pkt_safi = IANA_SAFI_UNICAST;

	if (DISABLE_BGP_ANNOUNCE)
		return NULL;

	if (bgp_debug_neighbor_events(peer))
		zlog_debug("send End-of-RIB for %s to %s",
			   get_afi_safi_str(afi, safi, false), peer->host);

	s = stream_new(peer->max_packet_size);

	/* Make BGP update packet. */
	bgp_packet_set_marker(s, BGP_MSG_UPDATE);

	/* Unfeasible Routes Length */
	stream_putw(s, 0);

	if (afi == AFI_IP && safi == SAFI_UNICAST) {
		/* Total Path Attribute Length */
		stream_putw(s, 0);
	} else {
		/* Convert AFI, SAFI to values for packet. */
		bgp_map_afi_safi_int2iana(afi, safi, &pkt_afi, &pkt_safi);

		/* Total Path Attribute Length */
		stream_putw(s, 6);
		stream_putc(s, BGP_ATTR_FLAG_OPTIONAL);
		stream_putc(s, BGP_ATTR_MP_UNREACH_NLRI);
		stream_putc(s, 3);
		stream_putw(s, pkt_afi);
		stream_putc(s, pkt_safi);
	}

	bgp_packet_set_size(s);
	return s;
}