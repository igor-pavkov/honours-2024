void usbnet_tx_timeout (struct net_device *net)
{
	struct usbnet		*dev = netdev_priv(net);

	unlink_urbs (dev, &dev->txq);
	tasklet_schedule (&dev->bh);
	/* this needs to be handled individually because the generic layer
	 * doesn't know what is sufficient and could not restore private
	 * information if a remedy of an unconditional reset were used.
	 */
	if (dev->driver_info->recover)
		(dev->driver_info->recover)(dev);
}