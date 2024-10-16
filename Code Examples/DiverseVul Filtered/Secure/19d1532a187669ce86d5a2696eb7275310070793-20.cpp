static void sixpack_write_wakeup(struct tty_struct *tty)
{
	struct sixpack *sp = sp_get(tty);
	int actual;

	if (!sp)
		return;
	if (sp->xleft <= 0)  {
		/* Now serial buffer is almost free & we can start
		 * transmission of another packet */
		sp->dev->stats.tx_packets++;
		clear_bit(TTY_DO_WRITE_WAKEUP, &tty->flags);
		sp->tx_enable = 0;
		netif_wake_queue(sp->dev);
		goto out;
	}

	if (sp->tx_enable) {
		actual = tty->ops->write(tty, sp->xhead, sp->xleft);
		sp->xleft -= actual;
		sp->xhead += actual;
	}

out:
	sp_put(sp);
}