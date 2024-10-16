static void rfcomm_tty_hangup(struct tty_struct *tty)
{
	struct rfcomm_dev *dev = (struct rfcomm_dev *) tty->driver_data;

	BT_DBG("tty %p dev %p", tty, dev);

	if (!dev)
		return;

	rfcomm_tty_flush_buffer(tty);

	if (test_bit(RFCOMM_RELEASE_ONHUP, &dev->flags)) {
		if (rfcomm_dev_get(dev->id) == NULL)
			return;
		rfcomm_dev_del(dev);
		tty_port_put(&dev->port);
	}
}