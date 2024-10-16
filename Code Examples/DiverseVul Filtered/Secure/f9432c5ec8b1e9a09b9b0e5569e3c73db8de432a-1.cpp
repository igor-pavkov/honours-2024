static int rfcomm_release_dev(void __user *arg)
{
	struct rfcomm_dev_req req;
	struct rfcomm_dev *dev;

	if (copy_from_user(&req, arg, sizeof(req)))
		return -EFAULT;

	BT_DBG("dev_id %d flags 0x%x", req.dev_id, req.flags);

	dev = rfcomm_dev_get(req.dev_id);
	if (!dev)
		return -ENODEV;

	if (dev->flags != NOCAP_FLAGS && !capable(CAP_NET_ADMIN)) {
		tty_port_put(&dev->port);
		return -EPERM;
	}

	if (req.flags & (1 << RFCOMM_HANGUP_NOW))
		rfcomm_dlc_close(dev->dlc, 0);

	/* Shut down TTY synchronously before freeing rfcomm_dev */
	if (dev->port.tty)
		tty_vhangup(dev->port.tty);

	if (!test_bit(RFCOMM_RELEASE_ONHUP, &dev->flags))
		rfcomm_dev_del(dev);
	tty_port_put(&dev->port);
	return 0;
}