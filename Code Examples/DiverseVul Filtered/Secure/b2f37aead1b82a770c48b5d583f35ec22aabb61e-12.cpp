static void mkiss_receive_buf(struct tty_struct *tty, const unsigned char *cp,
	const char *fp, int count)
{
	struct mkiss *ax = mkiss_get(tty);

	if (!ax)
		return;

	/*
	 * Argh! mtu change time! - costs us the packet part received
	 * at the change
	 */
	if (ax->mtu != ax->dev->mtu + 73)
		ax_changedmtu(ax);

	/* Read the characters out of the buffer */
	while (count--) {
		if (fp != NULL && *fp++) {
			if (!test_and_set_bit(AXF_ERROR, &ax->flags))
				ax->dev->stats.rx_errors++;
			cp++;
			continue;
		}

		kiss_unesc(ax, *cp++);
	}

	mkiss_put(ax);
	tty_unthrottle(tty);
}