void usbnet_defer_kevent (struct usbnet *dev, int work)
{
	set_bit (work, &dev->flags);
	if (!schedule_work (&dev->kevent)) {
		if (net_ratelimit())
			netdev_err(dev->net, "kevent %d may have been dropped\n", work);
	} else {
		netdev_dbg(dev->net, "kevent %d scheduled\n", work);
	}
}