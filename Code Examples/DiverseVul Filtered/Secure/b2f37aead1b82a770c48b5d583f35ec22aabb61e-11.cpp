static void ax_changedmtu(struct mkiss *ax)
{
	struct net_device *dev = ax->dev;
	unsigned char *xbuff, *rbuff, *oxbuff, *orbuff;
	int len;

	len = dev->mtu * 2;

	/*
	 * allow for arrival of larger UDP packets, even if we say not to
	 * also fixes a bug in which SunOS sends 512-byte packets even with
	 * an MSS of 128
	 */
	if (len < 576 * 2)
		len = 576 * 2;

	xbuff = kmalloc(len + 4, GFP_ATOMIC);
	rbuff = kmalloc(len + 4, GFP_ATOMIC);

	if (xbuff == NULL || rbuff == NULL)  {
		printk(KERN_ERR "mkiss: %s: unable to grow ax25 buffers, "
		       "MTU change cancelled.\n",
		       ax->dev->name);
		dev->mtu = ax->mtu;
		kfree(xbuff);
		kfree(rbuff);
		return;
	}

	spin_lock_bh(&ax->buflock);

	oxbuff    = ax->xbuff;
	ax->xbuff = xbuff;
	orbuff    = ax->rbuff;
	ax->rbuff = rbuff;

	if (ax->xleft) {
		if (ax->xleft <= len) {
			memcpy(ax->xbuff, ax->xhead, ax->xleft);
		} else  {
			ax->xleft = 0;
			dev->stats.tx_dropped++;
		}
	}

	ax->xhead = ax->xbuff;

	if (ax->rcount) {
		if (ax->rcount <= len) {
			memcpy(ax->rbuff, orbuff, ax->rcount);
		} else  {
			ax->rcount = 0;
			dev->stats.rx_over_errors++;
			set_bit(AXF_ERROR, &ax->flags);
		}
	}

	ax->mtu      = dev->mtu + 73;
	ax->buffsize = len;

	spin_unlock_bh(&ax->buflock);

	kfree(oxbuff);
	kfree(orbuff);
}