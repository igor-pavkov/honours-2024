static void bgp_packet_add(struct peer *peer, struct stream *s)
{
	intmax_t delta;
	uint32_t holdtime;

	frr_with_mutex (&peer->io_mtx) {
		/* if the queue is empty, reset the "last OK" timestamp to
		 * now, otherwise if we write another packet immediately
		 * after it'll get confused
		 */
		if (!stream_fifo_count_safe(peer->obuf))
			peer->last_sendq_ok = bgp_clock();

		stream_fifo_push(peer->obuf, s);

		delta = bgp_clock() - peer->last_sendq_ok;
		holdtime = atomic_load_explicit(&peer->holdtime,
						memory_order_relaxed);

		/* Note that when we're here, we're adding some packet to the
		 * OutQ.  That includes keepalives when there is nothing to
		 * do, so there's a guarantee we pass by here once in a while.
		 *
		 * That implies there is no need to go set up another separate
		 * timer that ticks down SendHoldTime, as we'll be here sooner
		 * or later anyway and will see the checks below failing.
		 */
		if (!holdtime) {
			/* no holdtime, do nothing. */
		} else if (delta > 2 * (intmax_t)holdtime) {
			flog_err(
				EC_BGP_SENDQ_STUCK_PROPER,
				"%s has not made any SendQ progress for 2 holdtimes, terminating session",
				peer->host);
			BGP_EVENT_ADD(peer, TCP_fatal_error);
		} else if (delta > (intmax_t)holdtime &&
			   bgp_clock() - peer->last_sendq_warn > 5) {
			flog_warn(
				EC_BGP_SENDQ_STUCK_WARN,
				"%s has not made any SendQ progress for 1 holdtime, peer overloaded?",
				peer->host);
			peer->last_sendq_warn = bgp_clock();
		}
	}
}