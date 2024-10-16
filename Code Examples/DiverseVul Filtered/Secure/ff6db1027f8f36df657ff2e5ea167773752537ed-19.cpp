static int bgp_notify_receive(struct peer *peer, bgp_size_t size)
{
	struct bgp_notify outer = {};
	struct bgp_notify inner = {};
	bool hard_reset = false;

	if (peer->notify.data) {
		XFREE(MTYPE_BGP_NOTIFICATION, peer->notify.data);
		peer->notify.length = 0;
		peer->notify.hard_reset = false;
	}

	outer.code = stream_getc(peer->curr);
	outer.subcode = stream_getc(peer->curr);
	outer.length = size - 2;
	outer.data = NULL;
	outer.raw_data = NULL;
	if (outer.length) {
		outer.raw_data = XMALLOC(MTYPE_BGP_NOTIFICATION, outer.length);
		memcpy(outer.raw_data, stream_pnt(peer->curr), outer.length);
	}

	hard_reset =
		bgp_notify_received_hard_reset(peer, outer.code, outer.subcode);
	if (hard_reset && outer.length) {
		inner = bgp_notify_decapsulate_hard_reset(&outer);
		peer->notify.hard_reset = true;
	} else {
		inner = outer;
	}

	/* Preserv notify code and sub code. */
	peer->notify.code = inner.code;
	peer->notify.subcode = inner.subcode;
	/* For further diagnostic record returned Data. */
	if (inner.length) {
		peer->notify.length = inner.length;
		peer->notify.data =
			XMALLOC(MTYPE_BGP_NOTIFICATION, inner.length);
		memcpy(peer->notify.data, inner.raw_data, inner.length);
	}

	/* For debug */
	{
		int i;
		int first = 0;
		char c[4];

		if (inner.length) {
			inner.data = XMALLOC(MTYPE_BGP_NOTIFICATION,
					     inner.length * 3);
			for (i = 0; i < inner.length; i++)
				if (first) {
					snprintf(c, sizeof(c), " %02x",
						stream_getc(peer->curr));

					strlcat(inner.data, c,
						inner.length * 3);

				} else {
					first = 1;
					snprintf(c, sizeof(c), "%02x",
						 stream_getc(peer->curr));

					strlcpy(inner.data, c,
						inner.length * 3);
				}
		}

		bgp_notify_print(peer, &inner, "received", hard_reset);
		if (inner.length) {
			XFREE(MTYPE_BGP_NOTIFICATION, inner.data);
			inner.length = 0;
		}
		if (outer.length) {
			XFREE(MTYPE_BGP_NOTIFICATION, outer.data);
			XFREE(MTYPE_BGP_NOTIFICATION, outer.raw_data);

			/* If this is a Hard Reset notification, we MUST free
			 * the inner (encapsulated) notification too.
			 */
			if (hard_reset)
				XFREE(MTYPE_BGP_NOTIFICATION, inner.raw_data);
			outer.length = 0;
		}
	}

	/* peer count update */
	atomic_fetch_add_explicit(&peer->notify_in, 1, memory_order_relaxed);

	peer->last_reset = PEER_DOWN_NOTIFY_RECEIVED;

	/* We have to check for Notify with Unsupported Optional Parameter.
	   in that case we fallback to open without the capability option.
	   But this done in bgp_stop. We just mark it here to avoid changing
	   the fsm tables.  */
	if (inner.code == BGP_NOTIFY_OPEN_ERR &&
	    inner.subcode == BGP_NOTIFY_OPEN_UNSUP_PARAM)
		UNSET_FLAG(peer->sflags, PEER_STATUS_CAPABILITY_OPEN);

	/* If Graceful-Restart N-bit (Notification) is exchanged,
	 * and it's not a Hard Reset, let's retain the routes.
	 */
	if (bgp_has_graceful_restart_notification(peer) && !hard_reset &&
	    CHECK_FLAG(peer->sflags, PEER_STATUS_NSF_MODE))
		SET_FLAG(peer->sflags, PEER_STATUS_NSF_WAIT);

	bgp_peer_gr_flags_update(peer);
	BGP_GR_ROUTER_DETECT_AND_SEND_CAPABILITY_TO_ZEBRA(peer->bgp,
							  peer->bgp->peer);

	return Receive_NOTIFICATION_message;
}