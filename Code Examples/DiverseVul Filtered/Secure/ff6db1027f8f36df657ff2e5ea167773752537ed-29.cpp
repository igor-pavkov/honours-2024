static int bgp_route_refresh_receive(struct peer *peer, bgp_size_t size)
{
	iana_afi_t pkt_afi;
	afi_t afi;
	iana_safi_t pkt_safi;
	safi_t safi;
	struct stream *s;
	struct peer_af *paf;
	struct update_group *updgrp;
	struct peer *updgrp_peer;
	uint8_t subtype;
	bool force_update = false;
	bgp_size_t msg_length =
		size - (BGP_MSG_ROUTE_REFRESH_MIN_SIZE - BGP_HEADER_SIZE);

	/* If peer does not have the capability, send notification. */
	if (!CHECK_FLAG(peer->cap, PEER_CAP_REFRESH_ADV)) {
		flog_err(EC_BGP_NO_CAP,
			 "%s [Error] BGP route refresh is not enabled",
			 peer->host);
		bgp_notify_send(peer, BGP_NOTIFY_HEADER_ERR,
				BGP_NOTIFY_HEADER_BAD_MESTYPE);
		return BGP_Stop;
	}

	/* Status must be Established. */
	if (!peer_established(peer)) {
		flog_err(
			EC_BGP_INVALID_STATUS,
			"%s [Error] Route refresh packet received under status %s",
			peer->host,
			lookup_msg(bgp_status_msg, peer->status, NULL));
		bgp_notify_send(peer, BGP_NOTIFY_FSM_ERR,
				bgp_fsm_error_subcode(peer->status));
		return BGP_Stop;
	}

	s = peer->curr;

	/* Parse packet. */
	pkt_afi = stream_getw(s);
	subtype = stream_getc(s);
	pkt_safi = stream_getc(s);

	/* Convert AFI, SAFI to internal values and check. */
	if (bgp_map_afi_safi_iana2int(pkt_afi, pkt_safi, &afi, &safi)) {
		zlog_info(
			"%s REFRESH_REQ for unrecognized afi/safi: %s/%s - ignored",
			peer->host, iana_afi2str(pkt_afi),
			iana_safi2str(pkt_safi));
		return BGP_PACKET_NOOP;
	}

	if (size != BGP_MSG_ROUTE_REFRESH_MIN_SIZE - BGP_HEADER_SIZE) {
		uint8_t *end;
		uint8_t when_to_refresh;
		uint8_t orf_type;
		uint16_t orf_len;

		if (subtype) {
			/* If the length, excluding the fixed-size message
			 * header, of the received ROUTE-REFRESH message with
			 * Message Subtype 1 and 2 is not 4, then the BGP
			 * speaker MUST send a NOTIFICATION message with the
			 * Error Code of "ROUTE-REFRESH Message Error" and the
			 * subcode of "Invalid Message Length".
			 */
			if (msg_length != 4) {
				zlog_err(
					"%s Enhanced Route Refresh message length error",
					peer->host);
				bgp_notify_send(
					peer, BGP_NOTIFY_ROUTE_REFRESH_ERR,
					BGP_NOTIFY_ROUTE_REFRESH_INVALID_MSG_LEN);
			}

			/* When the BGP speaker receives a ROUTE-REFRESH message
			 * with a "Message Subtype" field other than 0, 1, or 2,
			 * it MUST ignore the received ROUTE-REFRESH message.
			 */
			if (subtype > 2)
				zlog_err(
					"%s Enhanced Route Refresh invalid subtype",
					peer->host);
		}

		if (msg_length < 5) {
			zlog_info("%s ORF route refresh length error",
				  peer->host);
			bgp_notify_send(peer, BGP_NOTIFY_CEASE,
					BGP_NOTIFY_SUBCODE_UNSPECIFIC);
			return BGP_Stop;
		}

		when_to_refresh = stream_getc(s);
		end = stream_pnt(s) + (size - 5);

		while ((stream_pnt(s) + 2) < end) {
			orf_type = stream_getc(s);
			orf_len = stream_getw(s);

			/* orf_len in bounds? */
			if ((stream_pnt(s) + orf_len) > end)
				break; /* XXX: Notify instead?? */
			if (orf_type == ORF_TYPE_PREFIX
			    || orf_type == ORF_TYPE_PREFIX_OLD) {
				uint8_t *p_pnt = stream_pnt(s);
				uint8_t *p_end = stream_pnt(s) + orf_len;
				struct orf_prefix orfp;
				uint8_t common = 0;
				uint32_t seq;
				int psize;
				char name[BUFSIZ];
				int ret = CMD_SUCCESS;

				if (bgp_debug_neighbor_events(peer)) {
					zlog_debug(
						"%pBP rcvd Prefixlist ORF(%d) length %d",
						peer, orf_type, orf_len);
				}

				/* we're going to read at least 1 byte of common
				 * ORF header,
				 * and 7 bytes of ORF Address-filter entry from
				 * the stream
				 */
				if (orf_len < 7)
					break;

				/* ORF prefix-list name */
				snprintf(name, sizeof(name), "%s.%d.%d",
					 peer->host, afi, safi);

				while (p_pnt < p_end) {
					/* If the ORF entry is malformed, want
					 * to read as much of it
					 * as possible without going beyond the
					 * bounds of the entry,
					 * to maximise debug information.
					 */
					int ok;
					memset(&orfp, 0, sizeof(orfp));
					common = *p_pnt++;
					/* after ++: p_pnt <= p_end */
					if (common
					    & ORF_COMMON_PART_REMOVE_ALL) {
						if (bgp_debug_neighbor_events(
							    peer))
							zlog_debug(
								"%pBP rcvd Remove-All pfxlist ORF request",
								peer);
						prefix_bgp_orf_remove_all(afi,
									  name);
						break;
					}
					ok = ((uint32_t)(p_end - p_pnt)
					      >= sizeof(uint32_t));
					if (ok) {
						memcpy(&seq, p_pnt,
						       sizeof(uint32_t));
						p_pnt += sizeof(uint32_t);
						orfp.seq = ntohl(seq);
					} else
						p_pnt = p_end;

					/* val checked in prefix_bgp_orf_set */
					if (p_pnt < p_end)
						orfp.ge = *p_pnt++;

					/* val checked in prefix_bgp_orf_set */
					if (p_pnt < p_end)
						orfp.le = *p_pnt++;

					if ((ok = (p_pnt < p_end)))
						orfp.p.prefixlen = *p_pnt++;

					/* afi checked already */
					orfp.p.family = afi2family(afi);

					/* 0 if not ok */
					psize = PSIZE(orfp.p.prefixlen);
					/* valid for family ? */
					if (psize > prefix_blen(&orfp.p)) {
						ok = 0;
						psize = prefix_blen(&orfp.p);
					}
					/* valid for packet ? */
					if (psize > (p_end - p_pnt)) {
						ok = 0;
						psize = p_end - p_pnt;
					}

					if (psize > 0)
						memcpy(&orfp.p.u.prefix, p_pnt,
						       psize);
					p_pnt += psize;

					if (bgp_debug_neighbor_events(peer)) {
						char buf[INET6_BUFSIZ];

						zlog_debug(
							"%pBP rcvd %s %s seq %u %s/%d ge %d le %d%s",
							peer,
							(common & ORF_COMMON_PART_REMOVE
								 ? "Remove"
								 : "Add"),
							(common & ORF_COMMON_PART_DENY
								 ? "deny"
								 : "permit"),
							orfp.seq,
							inet_ntop(
								orfp.p.family,
								&orfp.p.u.prefix,
								buf,
								INET6_BUFSIZ),
							orfp.p.prefixlen,
							orfp.ge, orfp.le,
							ok ? "" : " MALFORMED");
					}

					if (ok)
						ret = prefix_bgp_orf_set(
							name, afi, &orfp,
							(common & ORF_COMMON_PART_DENY
								 ? 0
								 : 1),
							(common & ORF_COMMON_PART_REMOVE
								 ? 0
								 : 1));

					if (!ok || (ok && ret != CMD_SUCCESS)) {
						zlog_info(
							"%pBP Received misformatted prefixlist ORF. Remove All pfxlist",
							peer);
						prefix_bgp_orf_remove_all(afi,
									  name);
						break;
					}
				}

				peer->orf_plist[afi][safi] =
					prefix_bgp_orf_lookup(afi, name);
			}
			stream_forward_getp(s, orf_len);
		}
		if (bgp_debug_neighbor_events(peer))
			zlog_debug("%pBP rcvd Refresh %s ORF request", peer,
				   when_to_refresh == REFRESH_DEFER
					   ? "Defer"
					   : "Immediate");
		if (when_to_refresh == REFRESH_DEFER)
			return BGP_PACKET_NOOP;
	}

	/* First update is deferred until ORF or ROUTE-REFRESH is received */
	if (CHECK_FLAG(peer->af_sflags[afi][safi],
		       PEER_STATUS_ORF_WAIT_REFRESH))
		UNSET_FLAG(peer->af_sflags[afi][safi],
			   PEER_STATUS_ORF_WAIT_REFRESH);

	paf = peer_af_find(peer, afi, safi);
	if (paf && paf->subgroup) {
		if (peer->orf_plist[afi][safi]) {
			updgrp = PAF_UPDGRP(paf);
			updgrp_peer = UPDGRP_PEER(updgrp);
			updgrp_peer->orf_plist[afi][safi] =
				peer->orf_plist[afi][safi];
		}

		/* Avoid supressing duplicate routes later
		 * when processing in subgroup_announce_table().
		 */
		force_update = true;

		/* If the peer is configured for default-originate clear the
		 * SUBGRP_STATUS_DEFAULT_ORIGINATE flag so that we will
		 * re-advertise the
		 * default
		 */
		if (CHECK_FLAG(paf->subgroup->sflags,
			       SUBGRP_STATUS_DEFAULT_ORIGINATE))
			UNSET_FLAG(paf->subgroup->sflags,
				   SUBGRP_STATUS_DEFAULT_ORIGINATE);
	}

	if (subtype == BGP_ROUTE_REFRESH_BORR) {
		/* A BGP speaker that has received the Graceful Restart
		 * Capability from its neighbor MUST ignore any BoRRs for
		 * an <AFI, SAFI> from the neighbor before the speaker
		 * receives the EoR for the given <AFI, SAFI> from the
		 * neighbor.
		 */
		if (CHECK_FLAG(peer->cap, PEER_CAP_RESTART_RCV)
		    && !CHECK_FLAG(peer->af_sflags[afi][safi],
				   PEER_STATUS_EOR_RECEIVED)) {
			if (bgp_debug_neighbor_events(peer))
				zlog_debug(
					"%pBP rcvd route-refresh (BoRR) for %s/%s before EoR",
					peer, afi2str(afi), safi2str(safi));
			return BGP_PACKET_NOOP;
		}

		if (peer->t_refresh_stalepath) {
			if (bgp_debug_neighbor_events(peer))
				zlog_debug(
					"%pBP rcvd route-refresh (BoRR) for %s/%s, whereas BoRR already received",
					peer, afi2str(afi), safi2str(safi));
			return BGP_PACKET_NOOP;
		}

		SET_FLAG(peer->af_sflags[afi][safi], PEER_STATUS_BORR_RECEIVED);
		UNSET_FLAG(peer->af_sflags[afi][safi],
			   PEER_STATUS_EORR_RECEIVED);

		/* When a BGP speaker receives a BoRR message from
		 * a peer, it MUST mark all the routes with the given
		 * Address Family Identifier and Subsequent Address
		 * Family Identifier, <AFI, SAFI> [RFC2918], from
		 * that peer as stale.
		 */
		if (peer_active_nego(peer)) {
			SET_FLAG(peer->af_sflags[afi][safi],
				 PEER_STATUS_ENHANCED_REFRESH);
			bgp_set_stale_route(peer, afi, safi);
		}

		if (peer_established(peer))
			thread_add_timer(bm->master,
					 bgp_refresh_stalepath_timer_expire,
					 paf, peer->bgp->stalepath_time,
					 &peer->t_refresh_stalepath);

		if (bgp_debug_neighbor_events(peer))
			zlog_debug(
				"%pBP rcvd route-refresh (BoRR) for %s/%s, triggering timer for %u seconds",
				peer, afi2str(afi), safi2str(safi),
				peer->bgp->stalepath_time);
	} else if (subtype == BGP_ROUTE_REFRESH_EORR) {
		if (!peer->t_refresh_stalepath) {
			zlog_err(
				"%pBP rcvd route-refresh (EoRR) for %s/%s, whereas no BoRR received",
				peer, afi2str(afi), safi2str(safi));
			return BGP_PACKET_NOOP;
		}

		THREAD_OFF(peer->t_refresh_stalepath);

		SET_FLAG(peer->af_sflags[afi][safi], PEER_STATUS_EORR_RECEIVED);
		UNSET_FLAG(peer->af_sflags[afi][safi],
			   PEER_STATUS_BORR_RECEIVED);

		if (bgp_debug_neighbor_events(peer))
			zlog_debug(
				"%pBP rcvd route-refresh (EoRR) for %s/%s, stopping BoRR timer",
				peer, afi2str(afi), safi2str(safi));

		if (peer->nsf[afi][safi])
			bgp_clear_stale_route(peer, afi, safi);
	} else {
		if (bgp_debug_neighbor_events(peer))
			zlog_debug(
				"%pBP rcvd route-refresh (REQUEST) for %s/%s",
				peer, afi2str(afi), safi2str(safi));

		/* In response to a "normal route refresh request" from the
		 * peer, the speaker MUST send a BoRR message.
		 */
		if (CHECK_FLAG(peer->cap, PEER_CAP_ENHANCED_RR_RCV)) {
			/* For a BGP speaker that supports the BGP Graceful
			 * Restart, it MUST NOT send a BoRR for an <AFI, SAFI>
			 * to a neighbor before it sends the EoR for the
			 * <AFI, SAFI> to the neighbor.
			 */
			if (!CHECK_FLAG(peer->af_sflags[afi][safi],
					PEER_STATUS_EOR_SEND)) {
				if (bgp_debug_neighbor_events(peer))
					zlog_debug(
						"%pBP rcvd route-refresh (REQUEST) for %s/%s before EoR",
						peer, afi2str(afi),
						safi2str(safi));
				return BGP_PACKET_NOOP;
			}

			bgp_route_refresh_send(peer, afi, safi, 0, 0, 0,
					       BGP_ROUTE_REFRESH_BORR);

			if (bgp_debug_neighbor_events(peer))
				zlog_debug(
					"%pBP sending route-refresh (BoRR) for %s/%s",
					peer, afi2str(afi), safi2str(safi));

			/* Set flag Ready-To-Send to know when we can send EoRR
			 * message.
			 */
			SET_FLAG(peer->af_sflags[afi][safi],
				 PEER_STATUS_BORR_SEND);
			UNSET_FLAG(peer->af_sflags[afi][safi],
				   PEER_STATUS_EORR_SEND);
		}
	}

	/* Perform route refreshment to the peer */
	bgp_announce_route(peer, afi, safi, force_update);

	/* No FSM action necessary */
	return BGP_PACKET_NOOP;
}