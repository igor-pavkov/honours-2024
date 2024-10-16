static struct rfcomm_dev *rfcomm_dev_get(int id)
{
	struct rfcomm_dev *dev;

	spin_lock(&rfcomm_dev_lock);

	dev = __rfcomm_dev_get(id);

	if (dev) {
		if (test_bit(RFCOMM_TTY_RELEASED, &dev->flags))
			dev = NULL;
		else
			tty_port_get(&dev->port);
	}

	spin_unlock(&rfcomm_dev_lock);

	return dev;
}