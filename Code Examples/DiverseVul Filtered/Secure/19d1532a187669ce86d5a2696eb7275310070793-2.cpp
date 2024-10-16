static void resync_tnc(struct timer_list *t)
{
	struct sixpack *sp = from_timer(sp, t, resync_t);
	static char resync_cmd = 0xe8;

	/* clear any data that might have been received */

	sp->rx_count = 0;
	sp->rx_count_cooked = 0;

	/* reset state machine */

	sp->status = 1;
	sp->status1 = 1;
	sp->status2 = 0;

	/* resync the TNC */

	sp->led_state = 0x60;
	sp->tty->ops->write(sp->tty, &sp->led_state, 1);
	sp->tty->ops->write(sp->tty, &resync_cmd, 1);


	/* Start resync timer again -- the TNC might be still absent */
	mod_timer(&sp->resync_t, jiffies + SIXP_RESYNC_TIMEOUT);
}