void usbnet_disconnect (struct usb_interface *intf)
{
	struct usbnet		*dev;
	struct usb_device	*xdev;
	struct net_device	*net;

	dev = usb_get_intfdata(intf);
	usb_set_intfdata(intf, NULL);
	if (!dev)
		return;

	xdev = interface_to_usbdev (intf);

	netif_info(dev, probe, dev->net, "unregister '%s' usb-%s-%s, %s\n",
		   intf->dev.driver->name,
		   xdev->bus->bus_name, xdev->devpath,
		   dev->driver_info->description);

	net = dev->net;
	unregister_netdev (net);

	cancel_work_sync(&dev->kevent);

	usb_scuttle_anchored_urbs(&dev->deferred);

	if (dev->driver_info->unbind)
		dev->driver_info->unbind (dev, intf);

	usb_kill_urb(dev->interrupt);
	usb_free_urb(dev->interrupt);
	kfree(dev->padding_pkt);

	free_netdev(net);
}