void bgp_route_refresh_send(struct peer *peer, afi_t afi, safi_t safi,
			    uint8_t orf_type, uint8_t when_to_refresh,
			    int remove, uint8_t subtype)
{
	struct stream *s;
	struct bgp_filter *filter;
	int orf_refresh = 0;
	iana_afi_t pkt_afi = IANA_AFI_IPV4;
	iana_safi_t pkt_safi = IANA_SAFI_UNICAST;

	if (DISABLE_BGP_ANNOUNCE)
		return;

	filter = &peer->filter[afi][safi];

	/* Convert AFI, SAFI to values for packet. */
	bgp_map_afi_safi_int2iana(afi, safi, &pkt_afi, &pkt_safi);

	s = stream_new(peer->max_packet_size);

	/* Make BGP update packet. */
	if (CHECK_FLAG(peer->cap, PEER_CAP_REFRESH_NEW_RCV))
		bgp_packet_set_marker(s, BGP_MSG_ROUTE_REFRESH_NEW);
	else
		bgp_packet_set_marker(s, BGP_MSG_ROUTE_REFRESH_OLD);

	/* Encode Route Refresh message. */
	stream_putw(s, pkt_afi);
	if (subtype)
		stream_putc(s, subtype);
	else
		stream_putc(s, 0);
	stream_putc(s, pkt_safi);

	if (orf_type == ORF_TYPE_PREFIX || orf_type == ORF_TYPE_PREFIX_OLD)
		if (remove || filter->plist[FILTER_IN].plist) {
			uint16_t orf_len;
			unsigned long orfp;

			orf_refresh = 1;
			stream_putc(s, when_to_refresh);
			stream_putc(s, orf_type);
			orfp = stream_get_endp(s);
			stream_putw(s, 0);

			if (remove) {
				UNSET_FLAG(peer->af_sflags[afi][safi],
					   PEER_STATUS_ORF_PREFIX_SEND);
				stream_putc(s, ORF_COMMON_PART_REMOVE_ALL);
				if (bgp_debug_neighbor_events(peer))
					zlog_debug(
						"%pBP sending REFRESH_REQ to remove ORF(%d) (%s) for afi/safi: %s/%s",
						peer, orf_type,
						(when_to_refresh ==
								 REFRESH_DEFER
							 ? "defer"
							 : "immediate"),
						iana_afi2str(pkt_afi),
						iana_safi2str(pkt_safi));
			} else {
				SET_FLAG(peer->af_sflags[afi][safi],
					 PEER_STATUS_ORF_PREFIX_SEND);
				prefix_bgp_orf_entry(
					s, filter->plist[FILTER_IN].plist,
					ORF_COMMON_PART_ADD,
					ORF_COMMON_PART_PERMIT,
					ORF_COMMON_PART_DENY);
				if (bgp_debug_neighbor_events(peer))
					zlog_debug(
						"%pBP sending REFRESH_REQ with pfxlist ORF(%d) (%s) for afi/safi: %s/%s",
						peer, orf_type,
						(when_to_refresh ==
								 REFRESH_DEFER
							 ? "defer"
							 : "immediate"),
						iana_afi2str(pkt_afi),
						iana_safi2str(pkt_safi));
			}

			/* Total ORF Entry Len. */
			orf_len = stream_get_endp(s) - orfp - 2;
			stream_putw_at(s, orfp, orf_len);
		}

	/* Set packet size. */
	bgp_packet_set_size(s);

	if (bgp_debug_neighbor_events(peer)) {
		if (!orf_refresh)
			zlog_debug(
				"%pBP sending REFRESH_REQ for afi/safi: %s/%s",
				peer, iana_afi2str(pkt_afi),
				iana_safi2str(pkt_safi));
	}

	/* Add packet to the peer. */
	bgp_packet_add(peer, s);

	bgp_writes_on(peer);
}