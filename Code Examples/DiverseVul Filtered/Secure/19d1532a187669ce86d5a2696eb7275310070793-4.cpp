static void sp_xmit_on_air(struct timer_list *t)
{
	struct sixpack *sp = from_timer(sp, t, tx_t);
	int actual, when = sp->slottime;
	static unsigned char random;

	random = random * 17 + 41;

	if (((sp->status1 & SIXP_DCD_MASK) == 0) && (random < sp->persistence)) {
		sp->led_state = 0x70;
		sp->tty->ops->write(sp->tty, &sp->led_state, 1);
		sp->tx_enable = 1;
		actual = sp->tty->ops->write(sp->tty, sp->xbuff, sp->status2);
		sp->xleft -= actual;
		sp->xhead += actual;
		sp->led_state = 0x60;
		sp->tty->ops->write(sp->tty, &sp->led_state, 1);
		sp->status2 = 0;
	} else
		mod_timer(&sp->tx_t, jiffies + ((when + 1) * HZ) / 100);
}