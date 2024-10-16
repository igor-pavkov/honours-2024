static void rfcomm_tty_unthrottle(struct tty_struct *tty)
{
	struct rfcomm_dev *dev = (struct rfcomm_dev *) tty->driver_data;

	BT_DBG("tty %p dev %p", tty, dev);

	rfcomm_dlc_unthrottle(dev->dlc);
}