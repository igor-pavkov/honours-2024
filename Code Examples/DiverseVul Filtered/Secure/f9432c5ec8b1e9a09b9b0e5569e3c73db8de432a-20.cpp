static void rfcomm_tty_close(struct tty_struct *tty, struct file *filp)
{
	struct rfcomm_dev *dev = (struct rfcomm_dev *) tty->driver_data;
	unsigned long flags;

	if (!dev)
		return;

	BT_DBG("tty %p dev %p dlc %p opened %d", tty, dev, dev->dlc,
						dev->port.count);

	spin_lock_irqsave(&dev->port.lock, flags);
	if (!--dev->port.count) {
		spin_unlock_irqrestore(&dev->port.lock, flags);
		if (dev->tty_dev->parent)
			device_move(dev->tty_dev, NULL, DPM_ORDER_DEV_LAST);

		/* Close DLC and dettach TTY */
		rfcomm_dlc_close(dev->dlc, 0);

		clear_bit(RFCOMM_TTY_ATTACHED, &dev->flags);

		rfcomm_dlc_lock(dev->dlc);
		tty->driver_data = NULL;
		dev->port.tty = NULL;
		rfcomm_dlc_unlock(dev->dlc);

		if (test_bit(RFCOMM_TTY_RELEASED, &dev->flags)) {
			spin_lock(&rfcomm_dev_lock);
			list_del_init(&dev->list);
			spin_unlock(&rfcomm_dev_lock);

			tty_port_put(&dev->port);
		}
	} else
		spin_unlock_irqrestore(&dev->port.lock, flags);

	tty_port_put(&dev->port);
}