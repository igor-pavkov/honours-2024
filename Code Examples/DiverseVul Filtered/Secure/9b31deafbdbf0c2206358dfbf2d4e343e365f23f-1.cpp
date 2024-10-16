static bool read_packet(struct msg_digest *md)
{
	const struct iface_port *ifp = md->iface;
	int packet_len;
	/* ??? this buffer seems *way* too big */
	u_int8_t bigbuffer[MAX_INPUT_UDP_SIZE];

	u_int8_t *_buffer = bigbuffer;
	union {
		struct sockaddr sa;
		struct sockaddr_in sa_in4;
		struct sockaddr_in6 sa_in6;
	} from
#if defined(HAVE_UDPFROMTO)
	, to
#endif
	;
	socklen_t from_len = sizeof(from);
#if defined(HAVE_UDPFROMTO)
	socklen_t to_len   = sizeof(to);
#endif
	err_t from_ugh = NULL;
	static const char undisclosed[] = "unknown source";

	happy(anyaddr(addrtypeof(&ifp->ip_addr), &md->sender));
	zero(&from.sa);

#if defined(HAVE_UDPFROMTO)
	packet_len = recvfromto(ifp->fd, bigbuffer,
				sizeof(bigbuffer), /*flags*/ 0,
				&from.sa, &from_len,
				&to.sa, &to_len);
#else
	packet_len = recvfrom(ifp->fd, bigbuffer,
			      sizeof(bigbuffer), /*flags*/ 0,
			      &from.sa, &from_len);
#endif

	/* we do not do anything with *to* addresses yet... we will */

	/* First: digest the from address.
	 * We presume that nothing here disturbs errno.
	 */
	if (packet_len == -1 &&
	    from_len == sizeof(from) &&
	    all_zero((const void *)&from.sa, sizeof(from))) {
		/* "from" is untouched -- not set by recvfrom */
		from_ugh = undisclosed;
	} else if (from_len   <
		   (int) (offsetof(struct sockaddr,
				   sa_family) + sizeof(from.sa.sa_family))) {
		from_ugh = "truncated";
	} else {
		const struct af_info *afi = aftoinfo(from.sa.sa_family);

		if (afi == NULL) {
			from_ugh = "unexpected Address Family";
		} else if (from_len != afi->sa_sz) {
			from_ugh = "wrong length";
		} else {
			switch (from.sa.sa_family) {
			case AF_INET:
				from_ugh = initaddr(
					(void *) &from.sa_in4.sin_addr,
					sizeof(from.sa_in4.sin_addr),
					AF_INET, &md->sender);
				setportof(from.sa_in4.sin_port, &md->sender);
				md->sender_port = ntohs(from.sa_in4.sin_port);
				break;
			case AF_INET6:
				from_ugh = initaddr(
					(void *) &from.sa_in6.sin6_addr,
					sizeof(from.sa_in6.
					       sin6_addr),
					AF_INET6, &md->sender);
				setportof(from.sa_in6.sin6_port, &md->sender);
				md->sender_port = ntohs(from.sa_in6.sin6_port);
				break;
			}
		}
	}

	/* now we report any actual I/O error */
	if (packet_len == -1) {
		if (from_ugh == undisclosed &&
		    errno == ECONNREFUSED) {
			/* Tone down scary message for vague event:
			 * We get "connection refused" in response to some
			 * datagram we sent, but we cannot tell which one.
			 */
			libreswan_log(
				"some IKE message we sent has been rejected with ECONNREFUSED (kernel supplied no details)");
		} else if (from_ugh != NULL) {
			log_errno((e,
				   "recvfrom on %s failed; Pluto cannot decode source sockaddr in rejection: %s",
				   ifp->ip_dev->id_rname, from_ugh));
		} else {
			log_errno((e, "recvfrom on %s from %s:%u failed",
				   ifp->ip_dev->id_rname,
				   ip_str(&md->sender),
				   (unsigned)md->sender_port));
		}

		return FALSE;
	} else if (from_ugh != NULL) {
		libreswan_log(
			"recvfrom on %s returned malformed source sockaddr: %s",
			ifp->ip_dev->id_rname, from_ugh);
		return FALSE;
	}
	cur_from = &md->sender;
	cur_from_port = md->sender_port;

	if (ifp->ike_float == TRUE) {
		u_int32_t non_esp;
		if (packet_len < (int)sizeof(u_int32_t)) {
			libreswan_log("recvfrom %s:%u too small packet (%d)",
				      ip_str(
					      cur_from), (unsigned) cur_from_port,
				      packet_len);
			return FALSE;
		}
		memcpy(&non_esp, _buffer, sizeof(u_int32_t));
		if (non_esp != 0) {
			libreswan_log("recvfrom %s:%u has no Non-ESP marker",
				      ip_str(
					      cur_from),
				      (unsigned) cur_from_port);
			return FALSE;
		}
		_buffer += sizeof(u_int32_t);
		packet_len -= sizeof(u_int32_t);
	}

	/* Clone actual message contents
	 * and set up md->packet_pbs to describe it.
	 */
	init_pbs(&md->packet_pbs
		 , clone_bytes(_buffer, packet_len,
			       "message buffer in comm_handle()")
		 , packet_len, "packet");

	DBG(DBG_RAW | DBG_CRYPT | DBG_PARSING | DBG_CONTROL,
	    DBG_log("*received %d bytes from %s:%u on %s (port=%d)",
		    (int) pbs_room(&md->packet_pbs),
		    ip_str(cur_from), (unsigned) cur_from_port,
		    ifp->ip_dev->id_rname,
		    ifp->port)
	    );

	DBG(DBG_RAW,
	    DBG_dump("", md->packet_pbs.start, pbs_room(&md->packet_pbs)));

	/* We think that in 2013 Feb, Apple iOS Racoon
	 * sometimes generates an extra useless buggy confusing
	 * Non ESP Marker
	 */
	{
		static const u_int8_t non_ESP_marker[NON_ESP_MARKER_SIZE] =
		{ 0x00, };

		if (md->iface->ike_float &&
		    pbs_left(&md->packet_pbs) >= NON_ESP_MARKER_SIZE &&
		    memcmp(md->packet_pbs.cur, non_ESP_marker,
			   NON_ESP_MARKER_SIZE) == 0) {
			bool happy = in_raw(NULL, NON_ESP_MARKER_SIZE,
					    &md->packet_pbs,
					    "spurious extra Non ESP Marker");
			libreswan_log(
				"Removed spurious non-esp marker from IKE packet - Racoon bug");
			passert(happy);
		}
	}

	if ((pbs_room(&md->packet_pbs) == 1) &&
	    (md->packet_pbs.start[0] == 0xff)) {
		/**
		 * NAT-T Keep-alive packets should be discared by kernel ESPinUDP
		 * layer. But boggus keep-alive packets (sent with a non-esp marker)
		 * can reach this point. Complain and discard them.
		 */
		DBG(DBG_NATT,
		    DBG_log(
			    "NAT-T keep-alive (boggus ?) should not reach this point. "
			    "Ignored. Sender: %s:%u", ip_str(cur_from),
			    (unsigned) cur_from_port);
		    );
		return FALSE;
	}

	return TRUE;
}