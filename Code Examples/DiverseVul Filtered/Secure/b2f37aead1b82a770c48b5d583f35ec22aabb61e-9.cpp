static int mkiss_ioctl(struct tty_struct *tty, struct file *file,
	unsigned int cmd, unsigned long arg)
{
	struct mkiss *ax = mkiss_get(tty);
	struct net_device *dev;
	unsigned int tmp, err;

	/* First make sure we're connected. */
	if (ax == NULL)
		return -ENXIO;
	dev = ax->dev;

	switch (cmd) {
	case SIOCGIFNAME:
		err = copy_to_user((void __user *) arg, ax->dev->name,
		                   strlen(ax->dev->name) + 1) ? -EFAULT : 0;
		break;

	case SIOCGIFENCAP:
		err = put_user(4, (int __user *) arg);
		break;

	case SIOCSIFENCAP:
		if (get_user(tmp, (int __user *) arg)) {
			err = -EFAULT;
			break;
		}

		ax->mode = tmp;
		dev->addr_len        = AX25_ADDR_LEN;
		dev->hard_header_len = AX25_KISS_HEADER_LEN +
		                       AX25_MAX_HEADER_LEN + 3;
		dev->type            = ARPHRD_AX25;

		err = 0;
		break;

	case SIOCSIFHWADDR: {
		char addr[AX25_ADDR_LEN];

		if (copy_from_user(&addr,
		                   (void __user *) arg, AX25_ADDR_LEN)) {
			err = -EFAULT;
			break;
		}

		netif_tx_lock_bh(dev);
		__dev_addr_set(dev, addr, AX25_ADDR_LEN);
		netif_tx_unlock_bh(dev);

		err = 0;
		break;
	}
	default:
		err = -ENOIOCTLCMD;
	}

	mkiss_put(ax);

	return err;
}