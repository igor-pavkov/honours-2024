void bgp_notify_send_with_data(struct peer *peer, uint8_t code,
			       uint8_t sub_code, uint8_t *data, size_t datalen)
{
	struct stream *s;
	bool hard_reset = bgp_notify_send_hard_reset(peer, code, sub_code);

	/* Lock I/O mutex to prevent other threads from pushing packets */
	frr_mutex_lock_autounlock(&peer->io_mtx);
	/* ============================================== */

	/* Allocate new stream. */
	s = stream_new(peer->max_packet_size);

	/* Make notify packet. */
	bgp_packet_set_marker(s, BGP_MSG_NOTIFY);

	/* Check if we should send Hard Reset Notification or not */
	if (hard_reset) {
		uint8_t *hard_reset_message = bgp_notify_encapsulate_hard_reset(
			code, sub_code, data, datalen);

		/* Hard Reset encapsulates another NOTIFICATION message
		 * in its data portion.
		 */
		stream_putc(s, BGP_NOTIFY_CEASE);
		stream_putc(s, BGP_NOTIFY_CEASE_HARD_RESET);
		stream_write(s, hard_reset_message, datalen + 2);

		XFREE(MTYPE_BGP_NOTIFICATION, hard_reset_message);
	} else {
		stream_putc(s, code);
		stream_putc(s, sub_code);
		if (data)
			stream_write(s, data, datalen);
	}

	/* Set BGP packet length. */
	bgp_packet_set_size(s);

	/* wipe output buffer */
	stream_fifo_clean(peer->obuf);

	/*
	 * If possible, store last packet for debugging purposes. This check is
	 * in place because we are sometimes called with a doppelganger peer,
	 * who tends to have a plethora of fields nulled out.
	 */
	if (peer->curr) {
		size_t packetsize = stream_get_endp(peer->curr);
		assert(packetsize <= peer->max_packet_size);
		memcpy(peer->last_reset_cause, peer->curr->data, packetsize);
		peer->last_reset_cause_size = packetsize;
	}

	/* For debug */
	{
		struct bgp_notify bgp_notify;
		int first = 0;
		int i;
		char c[4];

		bgp_notify.code = code;
		bgp_notify.subcode = sub_code;
		bgp_notify.data = NULL;
		bgp_notify.length = datalen;
		bgp_notify.raw_data = data;

		peer->notify.code = bgp_notify.code;
		peer->notify.subcode = bgp_notify.subcode;

		if (bgp_notify.length && data) {
			bgp_notify.data =
				XMALLOC(MTYPE_TMP, bgp_notify.length * 3);
			for (i = 0; i < bgp_notify.length; i++)
				if (first) {
					snprintf(c, sizeof(c), " %02x",
						 data[i]);

					strlcat(bgp_notify.data, c,
						bgp_notify.length);

				} else {
					first = 1;
					snprintf(c, sizeof(c), "%02x", data[i]);

					strlcpy(bgp_notify.data, c,
						bgp_notify.length);
				}
		}
		bgp_notify_print(peer, &bgp_notify, "sending", hard_reset);

		if (bgp_notify.data) {
			XFREE(MTYPE_TMP, bgp_notify.data);
			bgp_notify.length = 0;
		}
	}

	/* peer reset cause */
	if (code == BGP_NOTIFY_CEASE) {
		if (sub_code == BGP_NOTIFY_CEASE_ADMIN_RESET)
			peer->last_reset = PEER_DOWN_USER_RESET;
		else if (sub_code == BGP_NOTIFY_CEASE_ADMIN_SHUTDOWN)
			peer->last_reset = PEER_DOWN_USER_SHUTDOWN;
		else
			peer->last_reset = PEER_DOWN_NOTIFY_SEND;
	} else
		peer->last_reset = PEER_DOWN_NOTIFY_SEND;

	/* Add packet to peer's output queue */
	stream_fifo_push(peer->obuf, s);

	bgp_peer_gr_flags_update(peer);
	BGP_GR_ROUTER_DETECT_AND_SEND_CAPABILITY_TO_ZEBRA(peer->bgp,
							  peer->bgp->peer);

	bgp_write_notify(peer);
}