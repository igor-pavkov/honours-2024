static void rfcomm_tty_flush_buffer(struct tty_struct *tty)
{
	struct rfcomm_dev *dev = (struct rfcomm_dev *) tty->driver_data;

	BT_DBG("tty %p dev %p", tty, dev);

	if (!dev || !dev->dlc)
		return;

	skb_queue_purge(&dev->dlc->tx_queue);
	tty_wakeup(tty);
}