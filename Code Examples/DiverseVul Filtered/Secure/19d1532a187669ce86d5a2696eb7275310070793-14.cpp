static int sixpack_open(struct tty_struct *tty)
{
	char *rbuff = NULL, *xbuff = NULL;
	struct net_device *dev;
	struct sixpack *sp;
	unsigned long len;
	int err = 0;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	if (tty->ops->write == NULL)
		return -EOPNOTSUPP;

	dev = alloc_netdev(sizeof(struct sixpack), "sp%d", NET_NAME_UNKNOWN,
			   sp_setup);
	if (!dev) {
		err = -ENOMEM;
		goto out;
	}

	sp = netdev_priv(dev);
	sp->dev = dev;

	spin_lock_init(&sp->lock);
	refcount_set(&sp->refcnt, 1);
	init_completion(&sp->dead);

	/* !!! length of the buffers. MTU is IP MTU, not PACLEN!  */

	len = dev->mtu * 2;

	rbuff = kmalloc(len + 4, GFP_KERNEL);
	xbuff = kmalloc(len + 4, GFP_KERNEL);

	if (rbuff == NULL || xbuff == NULL) {
		err = -ENOBUFS;
		goto out_free;
	}

	spin_lock_bh(&sp->lock);

	sp->tty = tty;

	sp->rbuff	= rbuff;
	sp->xbuff	= xbuff;

	sp->mtu		= AX25_MTU + 73;
	sp->buffsize	= len;
	sp->rcount	= 0;
	sp->rx_count	= 0;
	sp->rx_count_cooked = 0;
	sp->xleft	= 0;

	sp->flags	= 0;		/* Clear ESCAPE & ERROR flags */

	sp->duplex	= 0;
	sp->tx_delay    = SIXP_TXDELAY;
	sp->persistence = SIXP_PERSIST;
	sp->slottime    = SIXP_SLOTTIME;
	sp->led_state   = 0x60;
	sp->status      = 1;
	sp->status1     = 1;
	sp->status2     = 0;
	sp->tx_enable   = 0;

	netif_start_queue(dev);

	timer_setup(&sp->tx_t, sp_xmit_on_air, 0);

	timer_setup(&sp->resync_t, resync_tnc, 0);

	spin_unlock_bh(&sp->lock);

	/* Done.  We have linked the TTY line to a channel. */
	tty->disc_data = sp;
	tty->receive_room = 65536;

	/* Now we're ready to register. */
	err = register_netdev(dev);
	if (err)
		goto out_free;

	tnc_init(sp);

	return 0;

out_free:
	kfree(xbuff);
	kfree(rbuff);

	free_netdev(dev);

out:
	return err;
}