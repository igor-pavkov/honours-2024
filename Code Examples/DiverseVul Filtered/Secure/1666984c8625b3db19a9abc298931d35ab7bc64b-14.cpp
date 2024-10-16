int usbnet_stop (struct net_device *net)
{
	struct usbnet		*dev = netdev_priv(net);
	struct driver_info	*info = dev->driver_info;
	int			retval, pm, mpn;

	clear_bit(EVENT_DEV_OPEN, &dev->flags);
	netif_stop_queue (net);

	netif_info(dev, ifdown, dev->net,
		   "stop stats: rx/tx %lu/%lu, errs %lu/%lu\n",
		   net->stats.rx_packets, net->stats.tx_packets,
		   net->stats.rx_errors, net->stats.tx_errors);

	/* to not race resume */
	pm = usb_autopm_get_interface(dev->intf);
	/* allow minidriver to stop correctly (wireless devices to turn off
	 * radio etc) */
	if (info->stop) {
		retval = info->stop(dev);
		if (retval < 0)
			netif_info(dev, ifdown, dev->net,
				   "stop fail (%d) usbnet usb-%s-%s, %s\n",
				   retval,
				   dev->udev->bus->bus_name, dev->udev->devpath,
				   info->description);
	}

	if (!(info->flags & FLAG_AVOID_UNLINK_URBS))
		usbnet_terminate_urbs(dev);

	usbnet_status_stop(dev);

	usbnet_purge_paused_rxq(dev);

	mpn = !test_and_clear_bit(EVENT_NO_RUNTIME_PM, &dev->flags);

	/* deferred work (task, timer, softirq) must also stop.
	 * can't flush_scheduled_work() until we drop rtnl (later),
	 * else workers could deadlock; so make workers a NOP.
	 */
	dev->flags = 0;
	del_timer_sync (&dev->delay);
	tasklet_kill (&dev->bh);
	if (!pm)
		usb_autopm_put_interface(dev->intf);

	if (info->manage_power && mpn)
		info->manage_power(dev, 0);
	else
		usb_autopm_put_interface(dev->intf);

	return 0;
}