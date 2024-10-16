static void n_tty_set_room(struct tty_struct *tty)
{
	struct n_tty_data *ldata = tty->disc_data;

	/* Did this open up the receive buffer? We may need to flip */
	if (unlikely(ldata->no_room) && receive_room(tty)) {
		ldata->no_room = 0;

		WARN_RATELIMIT(tty->port->itty == NULL,
				"scheduling with invalid itty\n");
		/* see if ldisc has been killed - if so, this means that
		 * even though the ldisc has been halted and ->buf.work
		 * cancelled, ->buf.work is about to be rescheduled
		 */
		WARN_RATELIMIT(test_bit(TTY_LDISC_HALTED, &tty->flags),
			       "scheduling buffer work for halted ldisc\n");
		queue_work(system_unbound_wq, &tty->port->buf.work);
	}
}