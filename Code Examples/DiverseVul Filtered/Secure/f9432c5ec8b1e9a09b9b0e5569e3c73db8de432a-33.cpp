static int rfcomm_tty_open(struct tty_struct *tty, struct file *filp)
{
	DECLARE_WAITQUEUE(wait, current);
	struct rfcomm_dev *dev;
	struct rfcomm_dlc *dlc;
	unsigned long flags;
	int err, id;

	id = tty->index;

	BT_DBG("tty %p id %d", tty, id);

	/* We don't leak this refcount. For reasons which are not entirely
	   clear, the TTY layer will call our ->close() method even if the
	   open fails. We decrease the refcount there, and decreasing it
	   here too would cause breakage. */
	dev = rfcomm_dev_get(id);
	if (!dev)
		return -ENODEV;

	BT_DBG("dev %p dst %s channel %d opened %d", dev, batostr(&dev->dst),
				dev->channel, dev->port.count);

	spin_lock_irqsave(&dev->port.lock, flags);
	if (++dev->port.count > 1) {
		spin_unlock_irqrestore(&dev->port.lock, flags);
		return 0;
	}
	spin_unlock_irqrestore(&dev->port.lock, flags);

	dlc = dev->dlc;

	/* Attach TTY and open DLC */

	rfcomm_dlc_lock(dlc);
	tty->driver_data = dev;
	dev->port.tty = tty;
	rfcomm_dlc_unlock(dlc);
	set_bit(RFCOMM_TTY_ATTACHED, &dev->flags);

	err = rfcomm_dlc_open(dlc, &dev->src, &dev->dst, dev->channel);
	if (err < 0)
		return err;

	/* Wait for DLC to connect */
	add_wait_queue(&dev->wait, &wait);
	while (1) {
		set_current_state(TASK_INTERRUPTIBLE);

		if (dlc->state == BT_CLOSED) {
			err = -dev->err;
			break;
		}

		if (dlc->state == BT_CONNECTED)
			break;

		if (signal_pending(current)) {
			err = -EINTR;
			break;
		}

		tty_unlock();
		schedule();
		tty_lock();
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&dev->wait, &wait);

	if (err == 0)
		device_move(dev->tty_dev, rfcomm_get_device(dev),
			    DPM_ORDER_DEV_AFTER_PARENT);

	rfcomm_tty_copy_pending(dev);

	rfcomm_dlc_unthrottle(dev->dlc);

	return err;
}