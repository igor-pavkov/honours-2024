static int rfcomm_tty_chars_in_buffer(struct tty_struct *tty)
{
	struct rfcomm_dev *dev = (struct rfcomm_dev *) tty->driver_data;

	BT_DBG("tty %p dev %p", tty, dev);

	if (!dev || !dev->dlc)
		return 0;

	if (!skb_queue_empty(&dev->dlc->tx_queue))
		return dev->dlc->mtu;

	return 0;
}