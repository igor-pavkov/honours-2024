static int mkiss_open(struct tty_struct *tty)
{
	struct net_device *dev;
	struct mkiss *ax;
	int err;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	if (tty->ops->write == NULL)
		return -EOPNOTSUPP;

	dev = alloc_netdev(sizeof(struct mkiss), "ax%d", NET_NAME_UNKNOWN,
			   ax_setup);
	if (!dev) {
		err = -ENOMEM;
		goto out;
	}

	ax = netdev_priv(dev);
	ax->dev = dev;

	spin_lock_init(&ax->buflock);
	refcount_set(&ax->refcnt, 1);
	init_completion(&ax->dead);

	ax->tty = tty;
	tty->disc_data = ax;
	tty->receive_room = 65535;

	tty_driver_flush_buffer(tty);

	/* Restore default settings */
	dev->type = ARPHRD_AX25;

	/* Perform the low-level AX25 initialization. */
	err = ax_open(ax->dev);
	if (err)
		goto out_free_netdev;

	err = register_netdev(dev);
	if (err)
		goto out_free_buffers;

	/* after register_netdev() - because else printk smashes the kernel */
	switch (crc_force) {
	case 3:
		ax->crcmode  = CRC_MODE_SMACK;
		printk(KERN_INFO "mkiss: %s: crc mode smack forced.\n",
		       ax->dev->name);
		break;
	case 2:
		ax->crcmode  = CRC_MODE_FLEX;
		printk(KERN_INFO "mkiss: %s: crc mode flexnet forced.\n",
		       ax->dev->name);
		break;
	case 1:
		ax->crcmode  = CRC_MODE_NONE;
		printk(KERN_INFO "mkiss: %s: crc mode disabled.\n",
		       ax->dev->name);
		break;
	case 0:
	default:
		crc_force = 0;
		printk(KERN_INFO "mkiss: %s: crc mode is auto.\n",
		       ax->dev->name);
		ax->crcmode  = CRC_MODE_SMACK_TEST;
	}
	ax->crcauto = (crc_force ? 0 : 1);

	netif_start_queue(dev);

	/* Done.  We have linked the TTY line to a channel. */
	return 0;

out_free_buffers:
	kfree(ax->rbuff);
	kfree(ax->xbuff);

out_free_netdev:
	free_netdev(dev);

out:
	return err;
}