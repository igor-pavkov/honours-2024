static int bgp_update_receive(struct peer *peer, bgp_size_t size)
{
	int ret, nlri_ret;
	uint8_t *end;
	struct stream *s;
	struct attr attr;
	bgp_size_t attribute_len;
	bgp_size_t update_len;
	bgp_size_t withdraw_len;
	bool restart = false;

	enum NLRI_TYPES {
		NLRI_UPDATE,
		NLRI_WITHDRAW,
		NLRI_MP_UPDATE,
		NLRI_MP_WITHDRAW,
		NLRI_TYPE_MAX
	};
	struct bgp_nlri nlris[NLRI_TYPE_MAX];

	/* Status must be Established. */
	if (!peer_established(peer)) {
		flog_err(EC_BGP_INVALID_STATUS,
			 "%s [FSM] Update packet received under status %s",
			 peer->host,
			 lookup_msg(bgp_status_msg, peer->status, NULL));
		bgp_notify_send(peer, BGP_NOTIFY_FSM_ERR,
				bgp_fsm_error_subcode(peer->status));
		return BGP_Stop;
	}

	/* Set initial values. */
	memset(&attr, 0, sizeof(attr));
	attr.label_index = BGP_INVALID_LABEL_INDEX;
	attr.label = MPLS_INVALID_LABEL;
	memset(&nlris, 0, sizeof(nlris));
	memset(peer->rcvd_attr_str, 0, BUFSIZ);
	peer->rcvd_attr_printed = 0;

	s = peer->curr;
	end = stream_pnt(s) + size;

	/* RFC1771 6.3 If the Unfeasible Routes Length or Total Attribute
	   Length is too large (i.e., if Unfeasible Routes Length + Total
	   Attribute Length + 23 exceeds the message Length), then the Error
	   Subcode is set to Malformed Attribute List.  */
	if (stream_pnt(s) + 2 > end) {
		flog_err(EC_BGP_UPDATE_RCV,
			 "%s [Error] Update packet error (packet length is short for unfeasible length)",
			 peer->host);
		bgp_notify_send(peer, BGP_NOTIFY_UPDATE_ERR,
				BGP_NOTIFY_UPDATE_MAL_ATTR);
		return BGP_Stop;
	}

	/* Unfeasible Route Length. */
	withdraw_len = stream_getw(s);

	/* Unfeasible Route Length check. */
	if (stream_pnt(s) + withdraw_len > end) {
		flog_err(EC_BGP_UPDATE_RCV,
			 "%s [Error] Update packet error (packet unfeasible length overflow %d)",
			 peer->host, withdraw_len);
		bgp_notify_send(peer, BGP_NOTIFY_UPDATE_ERR,
				BGP_NOTIFY_UPDATE_MAL_ATTR);
		return BGP_Stop;
	}

	/* Unfeasible Route packet format check. */
	if (withdraw_len > 0) {
		nlris[NLRI_WITHDRAW].afi = AFI_IP;
		nlris[NLRI_WITHDRAW].safi = SAFI_UNICAST;
		nlris[NLRI_WITHDRAW].nlri = stream_pnt(s);
		nlris[NLRI_WITHDRAW].length = withdraw_len;
		stream_forward_getp(s, withdraw_len);
	}

	/* Attribute total length check. */
	if (stream_pnt(s) + 2 > end) {
		flog_warn(
			EC_BGP_UPDATE_PACKET_SHORT,
			"%s [Error] Packet Error (update packet is short for attribute length)",
			peer->host);
		bgp_notify_send(peer, BGP_NOTIFY_UPDATE_ERR,
				BGP_NOTIFY_UPDATE_MAL_ATTR);
		return BGP_Stop;
	}

	/* Fetch attribute total length. */
	attribute_len = stream_getw(s);

	/* Attribute length check. */
	if (stream_pnt(s) + attribute_len > end) {
		flog_warn(
			EC_BGP_UPDATE_PACKET_LONG,
			"%s [Error] Packet Error (update packet attribute length overflow %d)",
			peer->host, attribute_len);
		bgp_notify_send(peer, BGP_NOTIFY_UPDATE_ERR,
				BGP_NOTIFY_UPDATE_MAL_ATTR);
		return BGP_Stop;
	}

	/* Certain attribute parsing errors should not be considered bad enough
	 * to reset the session for, most particularly any partial/optional
	 * attributes that have 'tunneled' over speakers that don't understand
	 * them. Instead we withdraw only the prefix concerned.
	 *
	 * Complicates the flow a little though..
	 */
	enum bgp_attr_parse_ret attr_parse_ret = BGP_ATTR_PARSE_PROCEED;
/* This define morphs the update case into a withdraw when lower levels
 * have signalled an error condition where this is best.
 */
#define NLRI_ATTR_ARG (attr_parse_ret != BGP_ATTR_PARSE_WITHDRAW ? &attr : NULL)

	/* Parse attribute when it exists. */
	if (attribute_len) {
		attr_parse_ret = bgp_attr_parse(peer, &attr, attribute_len,
						&nlris[NLRI_MP_UPDATE],
						&nlris[NLRI_MP_WITHDRAW]);
		if (attr_parse_ret == BGP_ATTR_PARSE_ERROR) {
			bgp_attr_unintern_sub(&attr);
			return BGP_Stop;
		}
	}

	/* Logging the attribute. */
	if (attr_parse_ret == BGP_ATTR_PARSE_WITHDRAW
	    || BGP_DEBUG(update, UPDATE_IN)
	    || BGP_DEBUG(update, UPDATE_PREFIX)) {
		ret = bgp_dump_attr(&attr, peer->rcvd_attr_str,
				    sizeof(peer->rcvd_attr_str));

		peer->stat_upd_7606++;

		if (attr_parse_ret == BGP_ATTR_PARSE_WITHDRAW)
			flog_err(
				EC_BGP_UPDATE_RCV,
				"%pBP rcvd UPDATE with errors in attr(s)!! Withdrawing route.",
				peer);

		if (ret && bgp_debug_update(peer, NULL, NULL, 1)) {
			zlog_debug("%pBP rcvd UPDATE w/ attr: %s", peer,
				   peer->rcvd_attr_str);
			peer->rcvd_attr_printed = 1;
		}
	}

	/* Network Layer Reachability Information. */
	update_len = end - stream_pnt(s);

	if (update_len) {
		/* Set NLRI portion to structure. */
		nlris[NLRI_UPDATE].afi = AFI_IP;
		nlris[NLRI_UPDATE].safi = SAFI_UNICAST;
		nlris[NLRI_UPDATE].nlri = stream_pnt(s);
		nlris[NLRI_UPDATE].length = update_len;
		stream_forward_getp(s, update_len);

		if (CHECK_FLAG(attr.flag, ATTR_FLAG_BIT(BGP_ATTR_MP_REACH_NLRI))) {
			/*
			 * We skipped nexthop attribute validation earlier so
			 * validate the nexthop now.
			 */
			if (bgp_attr_nexthop_valid(peer, &attr) < 0) {
				bgp_attr_unintern_sub(&attr);
				return BGP_Stop;
			}
		}
	}

	if (BGP_DEBUG(update, UPDATE_IN))
		zlog_debug("%pBP rcvd UPDATE wlen %d attrlen %d alen %d", peer,
			   withdraw_len, attribute_len, update_len);

	/* Parse any given NLRIs */
	for (int i = NLRI_UPDATE; i < NLRI_TYPE_MAX; i++) {
		if (!nlris[i].nlri)
			continue;

		/* NLRI is processed iff the peer if configured for the specific
		 * afi/safi */
		if (!peer->afc[nlris[i].afi][nlris[i].safi]) {
			zlog_info(
				"%s [Info] UPDATE for non-enabled AFI/SAFI %u/%u",
				peer->host, nlris[i].afi, nlris[i].safi);
			continue;
		}

		/* EoR handled later */
		if (nlris[i].length == 0)
			continue;

		switch (i) {
		case NLRI_UPDATE:
		case NLRI_MP_UPDATE:
			nlri_ret = bgp_nlri_parse(peer, NLRI_ATTR_ARG,
						  &nlris[i], 0);
			break;
		case NLRI_WITHDRAW:
		case NLRI_MP_WITHDRAW:
			nlri_ret = bgp_nlri_parse(peer, &attr, &nlris[i], 1);
			break;
		default:
			nlri_ret = BGP_NLRI_PARSE_ERROR;
		}

		if (nlri_ret < BGP_NLRI_PARSE_OK
		    && nlri_ret != BGP_NLRI_PARSE_ERROR_PREFIX_OVERFLOW) {
			flog_err(EC_BGP_UPDATE_RCV,
				 "%s [Error] Error parsing NLRI", peer->host);
			if (peer_established(peer))
				bgp_notify_send(
					peer, BGP_NOTIFY_UPDATE_ERR,
					i <= NLRI_WITHDRAW
						? BGP_NOTIFY_UPDATE_INVAL_NETWORK
						: BGP_NOTIFY_UPDATE_OPT_ATTR_ERR);
			bgp_attr_unintern_sub(&attr);
			return BGP_Stop;
		}
	}

	/* EoR checks
	 *
	 * Non-MP IPv4/Unicast EoR is a completely empty UPDATE
	 * and MP EoR should have only an empty MP_UNREACH
	 */
	if ((!update_len && !withdraw_len && nlris[NLRI_MP_UPDATE].length == 0)
	    || (attr_parse_ret == BGP_ATTR_PARSE_EOR)) {
		afi_t afi = 0;
		safi_t safi;
		struct graceful_restart_info *gr_info;

		/* Restarting router */
		if (BGP_PEER_GRACEFUL_RESTART_CAPABLE(peer)
		    && BGP_PEER_RESTARTING_MODE(peer))
			restart = true;

		/* Non-MP IPv4/Unicast is a completely emtpy UPDATE - already
		 * checked
		 * update and withdraw NLRI lengths are 0.
		 */
		if (!attribute_len) {
			afi = AFI_IP;
			safi = SAFI_UNICAST;
		} else if (attr.flag & ATTR_FLAG_BIT(BGP_ATTR_MP_UNREACH_NLRI)
			   && nlris[NLRI_MP_WITHDRAW].length == 0) {
			afi = nlris[NLRI_MP_WITHDRAW].afi;
			safi = nlris[NLRI_MP_WITHDRAW].safi;
		} else if (attr_parse_ret == BGP_ATTR_PARSE_EOR) {
			afi = nlris[NLRI_MP_UPDATE].afi;
			safi = nlris[NLRI_MP_UPDATE].safi;
		}

		if (afi && peer->afc[afi][safi]) {
			struct vrf *vrf = vrf_lookup_by_id(peer->bgp->vrf_id);

			/* End-of-RIB received */
			if (!CHECK_FLAG(peer->af_sflags[afi][safi],
					PEER_STATUS_EOR_RECEIVED)) {
				SET_FLAG(peer->af_sflags[afi][safi],
					 PEER_STATUS_EOR_RECEIVED);
				bgp_update_explicit_eors(peer);
				/* Update graceful restart information */
				gr_info = &(peer->bgp->gr_info[afi][safi]);
				if (restart)
					gr_info->eor_received++;
				/* If EOR received from all peers and selection
				 * deferral timer is running, cancel the timer
				 * and invoke the best path calculation
				 */
				if (gr_info->eor_required
				    == gr_info->eor_received) {
					if (bgp_debug_neighbor_events(peer))
						zlog_debug(
							"%s %d, %s %d",
							"EOR REQ",
							gr_info->eor_required,
							"EOR RCV",
							gr_info->eor_received);
					THREAD_OFF(gr_info->t_select_deferral);
					gr_info->eor_required = 0;
					gr_info->eor_received = 0;
					/* Best path selection */
					if (bgp_best_path_select_defer(
						    peer->bgp, afi, safi)
					    < 0)
						return BGP_Stop;
				}
			}

			/* NSF delete stale route */
			if (peer->nsf[afi][safi])
				bgp_clear_stale_route(peer, afi, safi);

			zlog_info(
				"%s: rcvd End-of-RIB for %s from %s in vrf %s",
				__func__, get_afi_safi_str(afi, safi, false),
				peer->host, vrf ? vrf->name : VRF_DEFAULT_NAME);
		}
	}

	/* Everything is done.  We unintern temporary structures which
	   interned in bgp_attr_parse(). */
	bgp_attr_unintern_sub(&attr);

	peer->update_time = bgp_clock();

	/* Notify BGP Conditional advertisement scanner process */
	peer->advmap_table_change = true;

	return Receive_UPDATE_message;
}