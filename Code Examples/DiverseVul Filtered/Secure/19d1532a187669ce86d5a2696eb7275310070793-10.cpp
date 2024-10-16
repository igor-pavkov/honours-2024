static void sp_encaps(struct sixpack *sp, unsigned char *icp, int len)
{
	unsigned char *msg, *p = icp;
	int actual, count;

	if (len > sp->mtu) {	/* sp->mtu = AX25_MTU = max. PACLEN = 256 */
		msg = "oversized transmit packet!";
		goto out_drop;
	}

	if (p[0] > 5) {
		msg = "invalid KISS command";
		goto out_drop;
	}

	if ((p[0] != 0) && (len > 2)) {
		msg = "KISS control packet too long";
		goto out_drop;
	}

	if ((p[0] == 0) && (len < 15)) {
		msg = "bad AX.25 packet to transmit";
		goto out_drop;
	}

	count = encode_sixpack(p, sp->xbuff, len, sp->tx_delay);
	set_bit(TTY_DO_WRITE_WAKEUP, &sp->tty->flags);

	switch (p[0]) {
	case 1:	sp->tx_delay = p[1];
		return;
	case 2:	sp->persistence = p[1];
		return;
	case 3:	sp->slottime = p[1];
		return;
	case 4:	/* ignored */
		return;
	case 5:	sp->duplex = p[1];
		return;
	}

	if (p[0] != 0)
		return;

	/*
	 * In case of fullduplex or DAMA operation, we don't take care about the
	 * state of the DCD or of any timers, as the determination of the
	 * correct time to send is the job of the AX.25 layer. We send
	 * immediately after data has arrived.
	 */
	if (sp->duplex == 1) {
		sp->led_state = 0x70;
		sp->tty->ops->write(sp->tty, &sp->led_state, 1);
		sp->tx_enable = 1;
		actual = sp->tty->ops->write(sp->tty, sp->xbuff, count);
		sp->xleft = count - actual;
		sp->xhead = sp->xbuff + actual;
		sp->led_state = 0x60;
		sp->tty->ops->write(sp->tty, &sp->led_state, 1);
	} else {
		sp->xleft = count;
		sp->xhead = sp->xbuff;
		sp->status2 = count;
		sp_xmit_on_air(&sp->tx_t);
	}

	return;

out_drop:
	sp->dev->stats.tx_dropped++;
	netif_start_queue(sp->dev);
	if (net_ratelimit())
		printk(KERN_DEBUG "%s: %s - dropped.\n", sp->dev->name, msg);
}