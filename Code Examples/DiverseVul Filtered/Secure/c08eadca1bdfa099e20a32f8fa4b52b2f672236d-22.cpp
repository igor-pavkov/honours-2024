static int em28xx_duplicate_dev(struct em28xx *dev)
{
	int nr;
	struct em28xx *sec_dev = kmemdup(dev, sizeof(*sec_dev), GFP_KERNEL);

	if (!sec_dev) {
		dev->dev_next = NULL;
		return -ENOMEM;
	}
	/* Check to see next free device and mark as used */
	do {
		nr = find_first_zero_bit(em28xx_devused, EM28XX_MAXBOARDS);
		if (nr >= EM28XX_MAXBOARDS) {
			/* No free device slots */
			dev_warn(&dev->intf->dev, ": Supports only %i em28xx boards.\n",
				 EM28XX_MAXBOARDS);
			kfree(sec_dev);
			dev->dev_next = NULL;
			return -ENOMEM;
		}
	} while (test_and_set_bit(nr, em28xx_devused));
	sec_dev->devno = nr;
	snprintf(sec_dev->name, 28, "em28xx #%d", nr);
	sec_dev->dev_next = NULL;
	dev->dev_next = sec_dev;
	return 0;
}