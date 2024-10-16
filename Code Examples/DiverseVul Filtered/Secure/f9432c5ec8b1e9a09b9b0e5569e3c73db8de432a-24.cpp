static void rfcomm_dev_del(struct rfcomm_dev *dev)
{
	unsigned long flags;
	BT_DBG("dev %p", dev);

	BUG_ON(test_and_set_bit(RFCOMM_TTY_RELEASED, &dev->flags));

	spin_lock_irqsave(&dev->port.lock, flags);
	if (dev->port.count > 0) {
		spin_unlock_irqrestore(&dev->port.lock, flags);
		return;
	}
	spin_unlock_irqrestore(&dev->port.lock, flags);

	spin_lock(&rfcomm_dev_lock);
	list_del_init(&dev->list);
	spin_unlock(&rfcomm_dev_lock);

	tty_port_put(&dev->port);
}