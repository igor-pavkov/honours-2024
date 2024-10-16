void bgp_process_packet(struct thread *thread)
{
	/* Yes first of all get peer pointer. */
	struct peer *peer;	// peer
	uint32_t rpkt_quanta_old; // how many packets to read
	int fsm_update_result;    // return code of bgp_event_update()
	int mprc;		  // message processing return code

	peer = THREAD_ARG(thread);
	rpkt_quanta_old = atomic_load_explicit(&peer->bgp->rpkt_quanta,
					       memory_order_relaxed);
	fsm_update_result = 0;

	/* Guard against scheduled events that occur after peer deletion. */
	if (peer->status == Deleted || peer->status == Clearing)
		return;

	unsigned int processed = 0;

	while (processed < rpkt_quanta_old) {
		uint8_t type = 0;
		bgp_size_t size;
		char notify_data_length[2];

		frr_with_mutex (&peer->io_mtx) {
			peer->curr = stream_fifo_pop(peer->ibuf);
		}

		if (peer->curr == NULL) // no packets to process, hmm...
			return;

		/* skip the marker and copy the packet length */
		stream_forward_getp(peer->curr, BGP_MARKER_SIZE);
		memcpy(notify_data_length, stream_pnt(peer->curr), 2);

		/* read in the packet length and type */
		size = stream_getw(peer->curr);
		type = stream_getc(peer->curr);

		hook_call(bgp_packet_dump, peer, type, size, peer->curr);

		/* adjust size to exclude the marker + length + type */
		size -= BGP_HEADER_SIZE;

		/* Read rest of the packet and call each sort of packet routine
		 */
		switch (type) {
		case BGP_MSG_OPEN:
			frrtrace(2, frr_bgp, open_process, peer, size);
			atomic_fetch_add_explicit(&peer->open_in, 1,
						  memory_order_relaxed);
			mprc = bgp_open_receive(peer, size);
			if (mprc == BGP_Stop)
				flog_err(
					EC_BGP_PKT_OPEN,
					"%s: BGP OPEN receipt failed for peer: %s",
					__func__, peer->host);
			break;
		case BGP_MSG_UPDATE:
			frrtrace(2, frr_bgp, update_process, peer, size);
			atomic_fetch_add_explicit(&peer->update_in, 1,
						  memory_order_relaxed);
			peer->readtime = monotime(NULL);
			mprc = bgp_update_receive(peer, size);
			if (mprc == BGP_Stop)
				flog_err(
					EC_BGP_UPDATE_RCV,
					"%s: BGP UPDATE receipt failed for peer: %s",
					__func__, peer->host);
			break;
		case BGP_MSG_NOTIFY:
			frrtrace(2, frr_bgp, notification_process, peer, size);
			atomic_fetch_add_explicit(&peer->notify_in, 1,
						  memory_order_relaxed);
			mprc = bgp_notify_receive(peer, size);
			if (mprc == BGP_Stop)
				flog_err(
					EC_BGP_NOTIFY_RCV,
					"%s: BGP NOTIFY receipt failed for peer: %s",
					__func__, peer->host);
			break;
		case BGP_MSG_KEEPALIVE:
			frrtrace(2, frr_bgp, keepalive_process, peer, size);
			peer->readtime = monotime(NULL);
			atomic_fetch_add_explicit(&peer->keepalive_in, 1,
						  memory_order_relaxed);
			mprc = bgp_keepalive_receive(peer, size);
			if (mprc == BGP_Stop)
				flog_err(
					EC_BGP_KEEP_RCV,
					"%s: BGP KEEPALIVE receipt failed for peer: %s",
					__func__, peer->host);
			break;
		case BGP_MSG_ROUTE_REFRESH_NEW:
		case BGP_MSG_ROUTE_REFRESH_OLD:
			frrtrace(2, frr_bgp, refresh_process, peer, size);
			atomic_fetch_add_explicit(&peer->refresh_in, 1,
						  memory_order_relaxed);
			mprc = bgp_route_refresh_receive(peer, size);
			if (mprc == BGP_Stop)
				flog_err(
					EC_BGP_RFSH_RCV,
					"%s: BGP ROUTEREFRESH receipt failed for peer: %s",
					__func__, peer->host);
			break;
		case BGP_MSG_CAPABILITY:
			frrtrace(2, frr_bgp, capability_process, peer, size);
			atomic_fetch_add_explicit(&peer->dynamic_cap_in, 1,
						  memory_order_relaxed);
			mprc = bgp_capability_receive(peer, size);
			if (mprc == BGP_Stop)
				flog_err(
					EC_BGP_CAP_RCV,
					"%s: BGP CAPABILITY receipt failed for peer: %s",
					__func__, peer->host);
			break;
		default:
			/* Suppress uninitialized variable warning */
			mprc = 0;
			(void)mprc;
			/*
			 * The message type should have been sanitized before
			 * we ever got here. Receipt of a message with an
			 * invalid header at this point is indicative of a
			 * security issue.
			 */
			assert (!"Message of invalid type received during input processing");
		}

		/* delete processed packet */
		stream_free(peer->curr);
		peer->curr = NULL;
		processed++;

		/* Update FSM */
		if (mprc != BGP_PACKET_NOOP)
			fsm_update_result = bgp_event_update(peer, mprc);
		else
			continue;

		/*
		 * If peer was deleted, do not process any more packets. This
		 * is usually due to executing BGP_Stop or a stub deletion.
		 */
		if (fsm_update_result == FSM_PEER_TRANSFERRED
		    || fsm_update_result == FSM_PEER_STOPPED)
			break;
	}

	if (fsm_update_result != FSM_PEER_TRANSFERRED
	    && fsm_update_result != FSM_PEER_STOPPED) {
		frr_with_mutex (&peer->io_mtx) {
			// more work to do, come back later
			if (peer->ibuf->count > 0)
				thread_add_event(
					bm->master, bgp_process_packet, peer, 0,
					&peer->t_process_packet);
		}
	}
}