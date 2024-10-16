static int bgp_open_receive(struct peer *peer, bgp_size_t size)
{
	int ret;
	uint8_t version;
	uint16_t optlen;
	uint16_t holdtime;
	uint16_t send_holdtime;
	as_t remote_as;
	as_t as4 = 0, as4_be;
	struct in_addr remote_id;
	int mp_capability;
	uint8_t notify_data_remote_as[2];
	uint8_t notify_data_remote_as4[4];
	uint8_t notify_data_remote_id[4];
	uint16_t *holdtime_ptr;

	/* Parse open packet. */
	version = stream_getc(peer->curr);
	memcpy(notify_data_remote_as, stream_pnt(peer->curr), 2);
	remote_as = stream_getw(peer->curr);
	holdtime_ptr = (uint16_t *)stream_pnt(peer->curr);
	holdtime = stream_getw(peer->curr);
	memcpy(notify_data_remote_id, stream_pnt(peer->curr), 4);
	remote_id.s_addr = stream_get_ipv4(peer->curr);

	/* BEGIN to read the capability here, but dont do it yet */
	mp_capability = 0;
	optlen = stream_getc(peer->curr);

	/* Extended Optional Parameters Length for BGP OPEN Message */
	if (optlen == BGP_OPEN_NON_EXT_OPT_LEN
	    || CHECK_FLAG(peer->flags, PEER_FLAG_EXTENDED_OPT_PARAMS)) {
		uint8_t opttype;

		opttype = stream_getc(peer->curr);
		if (opttype == BGP_OPEN_NON_EXT_OPT_TYPE_EXTENDED_LENGTH) {
			optlen = stream_getw(peer->curr);
			SET_FLAG(peer->sflags,
				 PEER_STATUS_EXT_OPT_PARAMS_LENGTH);
		}
	}

	/* Receive OPEN message log  */
	if (bgp_debug_neighbor_events(peer))
		zlog_debug(
			"%s rcv OPEN%s, version %d, remote-as (in open) %u, holdtime %d, id %pI4",
			peer->host,
			CHECK_FLAG(peer->sflags,
				   PEER_STATUS_EXT_OPT_PARAMS_LENGTH)
				? " (Extended)"
				: "",
			version, remote_as, holdtime, &remote_id);

	if (optlen != 0) {
		/* If not enough bytes, it is an error. */
		if (STREAM_READABLE(peer->curr) < optlen) {
			flog_err(EC_BGP_PKT_OPEN,
				 "%s: stream has not enough bytes (%u)",
				 peer->host, optlen);
			bgp_notify_send(peer, BGP_NOTIFY_OPEN_ERR,
					BGP_NOTIFY_OPEN_MALFORMED_ATTR);
			return BGP_Stop;
		}

		/* We need the as4 capability value *right now* because
		 * if it is there, we have not got the remote_as yet, and
		 * without
		 * that we do not know which peer is connecting to us now.
		 */
		as4 = peek_for_as4_capability(peer, optlen);
	}

	as4_be = htonl(as4);
	memcpy(notify_data_remote_as4, &as4_be, 4);

	/* Just in case we have a silly peer who sends AS4 capability set to 0
	 */
	if (CHECK_FLAG(peer->cap, PEER_CAP_AS4_RCV) && !as4) {
		flog_err(EC_BGP_PKT_OPEN,
			 "%s bad OPEN, got AS4 capability, but AS4 set to 0",
			 peer->host);
		bgp_notify_send_with_data(peer, BGP_NOTIFY_OPEN_ERR,
					  BGP_NOTIFY_OPEN_BAD_PEER_AS,
					  notify_data_remote_as4, 4);
		return BGP_Stop;
	}

	/* Codification of AS 0 Processing */
	if (remote_as == BGP_AS_ZERO) {
		flog_err(EC_BGP_PKT_OPEN, "%s bad OPEN, got AS set to 0",
			 peer->host);
		bgp_notify_send(peer, BGP_NOTIFY_OPEN_ERR,
				BGP_NOTIFY_OPEN_BAD_PEER_AS);
		return BGP_Stop;
	}

	if (remote_as == BGP_AS_TRANS) {
		/* Take the AS4 from the capability.  We must have received the
		 * capability now!  Otherwise we have a asn16 peer who uses
		 * BGP_AS_TRANS, for some unknown reason.
		 */
		if (as4 == BGP_AS_TRANS) {
			flog_err(
				EC_BGP_PKT_OPEN,
				"%s [AS4] NEW speaker using AS_TRANS for AS4, not allowed",
				peer->host);
			bgp_notify_send_with_data(peer, BGP_NOTIFY_OPEN_ERR,
						  BGP_NOTIFY_OPEN_BAD_PEER_AS,
						  notify_data_remote_as4, 4);
			return BGP_Stop;
		}

		if (!as4 && BGP_DEBUG(as4, AS4))
			zlog_debug(
				"%s [AS4] OPEN remote_as is AS_TRANS, but no AS4. Odd, but proceeding.",
				peer->host);
		else if (as4 < BGP_AS_MAX && BGP_DEBUG(as4, AS4))
			zlog_debug(
				"%s [AS4] OPEN remote_as is AS_TRANS, but AS4 (%u) fits in 2-bytes, very odd peer.",
				peer->host, as4);
		if (as4)
			remote_as = as4;
	} else {
		/* We may have a partner with AS4 who has an asno < BGP_AS_MAX
		 */
		/* If we have got the capability, peer->as4cap must match
		 * remote_as */
		if (CHECK_FLAG(peer->cap, PEER_CAP_AS4_RCV)
		    && as4 != remote_as) {
			/* raise error, log this, close session */
			flog_err(
				EC_BGP_PKT_OPEN,
				"%s bad OPEN, got AS4 capability, but remote_as %u mismatch with 16bit 'myasn' %u in open",
				peer->host, as4, remote_as);
			bgp_notify_send_with_data(peer, BGP_NOTIFY_OPEN_ERR,
						  BGP_NOTIFY_OPEN_BAD_PEER_AS,
						  notify_data_remote_as4, 4);
			return BGP_Stop;
		}
	}

	/* rfc6286:
	 * If the BGP Identifier field of the OPEN message
	 * is zero, or if it is the same as the BGP Identifier
	 * of the local BGP speaker and the message is from an
	 * internal peer, then the Error Subcode is set to
	 * "Bad BGP Identifier".
	 */
	if (remote_id.s_addr == INADDR_ANY
	    || (peer->sort == BGP_PEER_IBGP
		&& ntohl(peer->local_id.s_addr) == ntohl(remote_id.s_addr))) {
		if (bgp_debug_neighbor_events(peer))
			zlog_debug("%s bad OPEN, wrong router identifier %pI4",
				   peer->host, &remote_id);
		bgp_notify_send_with_data(peer, BGP_NOTIFY_OPEN_ERR,
					  BGP_NOTIFY_OPEN_BAD_BGP_IDENT,
					  notify_data_remote_id, 4);
		return BGP_Stop;
	}

	/* Peer BGP version check. */
	if (version != BGP_VERSION_4) {
		uint16_t maxver = htons(BGP_VERSION_4);
		/* XXX this reply may not be correct if version < 4  XXX */
		if (bgp_debug_neighbor_events(peer))
			zlog_debug(
				"%s bad protocol version, remote requested %d, local request %d",
				peer->host, version, BGP_VERSION_4);
		/* Data must be in network byte order here */
		bgp_notify_send_with_data(peer, BGP_NOTIFY_OPEN_ERR,
					  BGP_NOTIFY_OPEN_UNSUP_VERSION,
					  (uint8_t *)&maxver, 2);
		return BGP_Stop;
	}

	/* Check neighbor as number. */
	if (peer->as_type == AS_UNSPECIFIED) {
		if (bgp_debug_neighbor_events(peer))
			zlog_debug(
				"%s bad OPEN, remote AS is unspecified currently",
				peer->host);
		bgp_notify_send_with_data(peer, BGP_NOTIFY_OPEN_ERR,
					  BGP_NOTIFY_OPEN_BAD_PEER_AS,
					  notify_data_remote_as, 2);
		return BGP_Stop;
	} else if (peer->as_type == AS_INTERNAL) {
		if (remote_as != peer->bgp->as) {
			if (bgp_debug_neighbor_events(peer))
				zlog_debug(
					"%s bad OPEN, remote AS is %u, internal specified",
					peer->host, remote_as);
			bgp_notify_send_with_data(peer, BGP_NOTIFY_OPEN_ERR,
						  BGP_NOTIFY_OPEN_BAD_PEER_AS,
						  notify_data_remote_as, 2);
			return BGP_Stop;
		}
		peer->as = peer->local_as;
	} else if (peer->as_type == AS_EXTERNAL) {
		if (remote_as == peer->bgp->as) {
			if (bgp_debug_neighbor_events(peer))
				zlog_debug(
					"%s bad OPEN, remote AS is %u, external specified",
					peer->host, remote_as);
			bgp_notify_send_with_data(peer, BGP_NOTIFY_OPEN_ERR,
						  BGP_NOTIFY_OPEN_BAD_PEER_AS,
						  notify_data_remote_as, 2);
			return BGP_Stop;
		}
		peer->as = remote_as;
	} else if ((peer->as_type == AS_SPECIFIED) && (remote_as != peer->as)) {
		if (bgp_debug_neighbor_events(peer))
			zlog_debug("%s bad OPEN, remote AS is %u, expected %u",
				   peer->host, remote_as, peer->as);
		bgp_notify_send_with_data(peer, BGP_NOTIFY_OPEN_ERR,
					  BGP_NOTIFY_OPEN_BAD_PEER_AS,
					  notify_data_remote_as, 2);
		return BGP_Stop;
	}

	/*
	 * When collision is detected and this peer is closed.
	 * Return immediately.
	 */
	ret = bgp_collision_detect(peer, remote_id);
	if (ret < 0)
		return BGP_Stop;

	/* Get sockname. */
	if (bgp_getsockname(peer) < 0) {
		flog_err_sys(EC_LIB_SOCKET,
			     "%s: bgp_getsockname() failed for peer: %s",
			     __func__, peer->host);
		return BGP_Stop;
	}

	/* Set remote router-id */
	peer->remote_id = remote_id;

	/* From the rfc: Upon receipt of an OPEN message, a BGP speaker MUST
	   calculate the value of the Hold Timer by using the smaller of its
	   configured Hold Time and the Hold Time received in the OPEN message.
	   The Hold Time MUST be either zero or at least three seconds.  An
	   implementation may reject connections on the basis of the Hold Time.
	   */

	if (holdtime < 3 && holdtime != 0) {
		bgp_notify_send_with_data(peer, BGP_NOTIFY_OPEN_ERR,
					  BGP_NOTIFY_OPEN_UNACEP_HOLDTIME,
					  (uint8_t *)holdtime_ptr, 2);
		return BGP_Stop;
	}

	/* Send notification message when Hold Time received in the OPEN message
	 * is smaller than configured minimum Hold Time. */
	if (holdtime < peer->bgp->default_min_holdtime
	    && peer->bgp->default_min_holdtime != 0) {
		bgp_notify_send_with_data(peer, BGP_NOTIFY_OPEN_ERR,
					  BGP_NOTIFY_OPEN_UNACEP_HOLDTIME,
					  (uint8_t *)holdtime_ptr, 2);
		return BGP_Stop;
	}

	/* From the rfc: A reasonable maximum time between KEEPALIVE messages
	   would be one third of the Hold Time interval.  KEEPALIVE messages
	   MUST NOT be sent more frequently than one per second.  An
	   implementation MAY adjust the rate at which it sends KEEPALIVE
	   messages as a function of the Hold Time interval. */

	if (CHECK_FLAG(peer->flags, PEER_FLAG_TIMER))
		send_holdtime = peer->holdtime;
	else
		send_holdtime = peer->bgp->default_holdtime;

	if (holdtime < send_holdtime)
		peer->v_holdtime = holdtime;
	else
		peer->v_holdtime = send_holdtime;

	/* Set effective keepalive to 1/3 the effective holdtime.
	 * Use configured keeplive when < effective keepalive.
	 */
	peer->v_keepalive = peer->v_holdtime / 3;
	if (CHECK_FLAG(peer->flags, PEER_FLAG_TIMER)) {
		if (peer->keepalive && peer->keepalive < peer->v_keepalive)
			peer->v_keepalive = peer->keepalive;
	} else {
		if (peer->bgp->default_keepalive
		    && peer->bgp->default_keepalive < peer->v_keepalive)
			peer->v_keepalive = peer->bgp->default_keepalive;
	}

	/* Open option part parse. */
	if (optlen != 0) {
		if (bgp_open_option_parse(peer, optlen, &mp_capability) < 0)
			return BGP_Stop;
	} else {
		if (bgp_debug_neighbor_events(peer))
			zlog_debug("%s rcvd OPEN w/ OPTION parameter len: 0",
				   peer->host);
	}

	/*
	 * Assume that the peer supports the locally configured set of
	 * AFI/SAFIs if the peer did not send us any Mulitiprotocol
	 * capabilities, or if 'override-capability' is configured.
	 */
	if (!mp_capability
	    || CHECK_FLAG(peer->flags, PEER_FLAG_OVERRIDE_CAPABILITY)) {
		peer->afc_nego[AFI_IP][SAFI_UNICAST] =
			peer->afc[AFI_IP][SAFI_UNICAST];
		peer->afc_nego[AFI_IP][SAFI_MULTICAST] =
			peer->afc[AFI_IP][SAFI_MULTICAST];
		peer->afc_nego[AFI_IP][SAFI_LABELED_UNICAST] =
			peer->afc[AFI_IP][SAFI_LABELED_UNICAST];
		peer->afc_nego[AFI_IP][SAFI_FLOWSPEC] =
			peer->afc[AFI_IP][SAFI_FLOWSPEC];
		peer->afc_nego[AFI_IP6][SAFI_UNICAST] =
			peer->afc[AFI_IP6][SAFI_UNICAST];
		peer->afc_nego[AFI_IP6][SAFI_MULTICAST] =
			peer->afc[AFI_IP6][SAFI_MULTICAST];
		peer->afc_nego[AFI_IP6][SAFI_LABELED_UNICAST] =
			peer->afc[AFI_IP6][SAFI_LABELED_UNICAST];
		peer->afc_nego[AFI_L2VPN][SAFI_EVPN] =
			peer->afc[AFI_L2VPN][SAFI_EVPN];
		peer->afc_nego[AFI_IP6][SAFI_FLOWSPEC] =
			peer->afc[AFI_IP6][SAFI_FLOWSPEC];
	}

	/* Verify valid local address present based on negotiated
	 * address-families. */
	if (peer->afc_nego[AFI_IP][SAFI_UNICAST]
	    || peer->afc_nego[AFI_IP][SAFI_LABELED_UNICAST]
	    || peer->afc_nego[AFI_IP][SAFI_MULTICAST]
	    || peer->afc_nego[AFI_IP][SAFI_MPLS_VPN]
	    || peer->afc_nego[AFI_IP][SAFI_ENCAP]) {
		if (peer->nexthop.v4.s_addr == INADDR_ANY) {
#if defined(HAVE_CUMULUS)
			zlog_warn("%s: No local IPv4 addr, BGP routing may not work",
				  peer->host);
#endif
		}
	}
	if (peer->afc_nego[AFI_IP6][SAFI_UNICAST]
	    || peer->afc_nego[AFI_IP6][SAFI_LABELED_UNICAST]
	    || peer->afc_nego[AFI_IP6][SAFI_MULTICAST]
	    || peer->afc_nego[AFI_IP6][SAFI_MPLS_VPN]
	    || peer->afc_nego[AFI_IP6][SAFI_ENCAP]) {
		if (IN6_IS_ADDR_UNSPECIFIED(&peer->nexthop.v6_global)) {
#if defined(HAVE_CUMULUS)
			zlog_warn("%s: No local IPv6 address, BGP routing may not work",
				  peer->host);
#endif
		}
	}
	peer->rtt = sockopt_tcp_rtt(peer->fd);

	return Receive_OPEN_message;
}