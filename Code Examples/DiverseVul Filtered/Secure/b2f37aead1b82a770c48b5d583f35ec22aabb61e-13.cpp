static void ax_encaps(struct net_device *dev, unsigned char *icp, int len)
{
	struct mkiss *ax = netdev_priv(dev);
	unsigned char *p;
	int actual, count;

	if (ax->mtu != ax->dev->mtu + 73)	/* Someone has been ifconfigging */
		ax_changedmtu(ax);

	if (len > ax->mtu) {		/* Sigh, shouldn't occur BUT ... */
		printk(KERN_ERR "mkiss: %s: truncating oversized transmit packet!\n", ax->dev->name);
		dev->stats.tx_dropped++;
		netif_start_queue(dev);
		return;
	}

	p = icp;

	spin_lock_bh(&ax->buflock);
	if ((*p & 0x0f) != 0) {
		/* Configuration Command (kissparms(1).
		 * Protocol spec says: never append CRC.
		 * This fixes a very old bug in the linux
		 * kiss driver. -- dl9sau */
		switch (*p & 0xff) {
		case 0x85:
			/* command from userspace especially for us,
			 * not for delivery to the tnc */
			if (len > 1) {
				int cmd = (p[1] & 0xff);
				switch(cmd) {
				case 3:
				  ax->crcmode = CRC_MODE_SMACK;
				  break;
				case 2:
				  ax->crcmode = CRC_MODE_FLEX;
				  break;
				case 1:
				  ax->crcmode = CRC_MODE_NONE;
				  break;
				case 0:
				default:
				  ax->crcmode = CRC_MODE_SMACK_TEST;
				  cmd = 0;
				}
				ax->crcauto = (cmd ? 0 : 1);
				printk(KERN_INFO "mkiss: %s: crc mode set to %d\n",
				       ax->dev->name, cmd);
			}
			spin_unlock_bh(&ax->buflock);
			netif_start_queue(dev);

			return;
		default:
			count = kiss_esc(p, ax->xbuff, len);
		}
	} else {
		unsigned short crc;
		switch (ax->crcmode) {
		case CRC_MODE_SMACK_TEST:
			ax->crcmode  = CRC_MODE_FLEX_TEST;
			printk(KERN_INFO "mkiss: %s: Trying crc-smack\n", ax->dev->name);
			fallthrough;
		case CRC_MODE_SMACK:
			*p |= 0x80;
			crc = swab16(crc16(0, p, len));
			count = kiss_esc_crc(p, ax->xbuff, crc, len+2);
			break;
		case CRC_MODE_FLEX_TEST:
			ax->crcmode = CRC_MODE_NONE;
			printk(KERN_INFO "mkiss: %s: Trying crc-flexnet\n", ax->dev->name);
			fallthrough;
		case CRC_MODE_FLEX:
			*p |= 0x20;
			crc = calc_crc_flex(p, len);
			count = kiss_esc_crc(p, ax->xbuff, crc, len+2);
			break;

		default:
			count = kiss_esc(p, ax->xbuff, len);
		}
	}
	spin_unlock_bh(&ax->buflock);

	set_bit(TTY_DO_WRITE_WAKEUP, &ax->tty->flags);
	actual = ax->tty->ops->write(ax->tty, ax->xbuff, count);
	dev->stats.tx_packets++;
	dev->stats.tx_bytes += actual;

	netif_trans_update(ax->dev);
	ax->xleft = count - actual;
	ax->xhead = ax->xbuff + actual;
}