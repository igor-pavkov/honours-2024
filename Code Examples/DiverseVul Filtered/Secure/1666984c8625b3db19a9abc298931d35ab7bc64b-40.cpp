static void __handle_link_change(struct usbnet *dev)
{
	if (!test_bit(EVENT_DEV_OPEN, &dev->flags))
		return;

	if (!netif_carrier_ok(dev->net)) {
		/* kill URBs for reading packets to save bus bandwidth */
		unlink_urbs(dev, &dev->rxq);

		/*
		 * tx_timeout will unlink URBs for sending packets and
		 * tx queue is stopped by netcore after link becomes off
		 */
	} else {
		/* submitting URBs for reading packets */
		tasklet_schedule(&dev->bh);
	}

	/* hard_mtu or rx_urb_size may change during link change */
	usbnet_update_max_qlen(dev);

	clear_bit(EVENT_LINK_CHANGE, &dev->flags);
}