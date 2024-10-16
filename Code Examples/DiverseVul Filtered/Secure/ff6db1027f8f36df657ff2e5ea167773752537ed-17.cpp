static void bgp_write_notify(struct peer *peer)
{
	int ret, val;
	uint8_t type;
	struct stream *s;

	/* There should be at least one packet. */
	s = stream_fifo_pop(peer->obuf);

	if (!s)
		return;

	assert(stream_get_endp(s) >= BGP_HEADER_SIZE);

	/*
	 * socket is in nonblocking mode, if we can't deliver the NOTIFY, well,
	 * we only care about getting a clean shutdown at this point.
	 */
	ret = write(peer->fd, STREAM_DATA(s), stream_get_endp(s));

	/*
	 * only connection reset/close gets counted as TCP_fatal_error, failure
	 * to write the entire NOTIFY doesn't get different FSM treatment
	 */
	if (ret <= 0) {
		stream_free(s);
		BGP_EVENT_ADD(peer, TCP_fatal_error);
		return;
	}

	/* Disable Nagle, make NOTIFY packet go out right away */
	val = 1;
	(void)setsockopt(peer->fd, IPPROTO_TCP, TCP_NODELAY, (char *)&val,
			 sizeof(val));

	/* Retrieve BGP packet type. */
	stream_set_getp(s, BGP_MARKER_SIZE + 2);
	type = stream_getc(s);

	assert(type == BGP_MSG_NOTIFY);

	/* Type should be notify. */
	atomic_fetch_add_explicit(&peer->notify_out, 1, memory_order_relaxed);

	/* Double start timer. */
	peer->v_start *= 2;

	/* Overflow check. */
	if (peer->v_start >= (60 * 2))
		peer->v_start = (60 * 2);

	/*
	 * Handle Graceful Restart case where the state changes to
	 * Connect instead of Idle
	 */
	BGP_EVENT_ADD(peer, BGP_Stop);

	stream_free(s);
}