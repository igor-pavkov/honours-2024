static int hub_port_wait_reset(struct usb_hub *hub, int port1,
			struct usb_device *udev, unsigned int delay, bool warm)
{
	int delay_time, ret;
	u16 portstatus;
	u16 portchange;

	for (delay_time = 0;
			delay_time < HUB_RESET_TIMEOUT;
			delay_time += delay) {
		/* wait to give the device a chance to reset */
		msleep(delay);

		/* read and decode port status */
		ret = hub_port_status(hub, port1, &portstatus, &portchange);
		if (ret < 0)
			return ret;

		/* The port state is unknown until the reset completes. */
		if (!(portstatus & USB_PORT_STAT_RESET))
			break;

		/* switch to the long delay after two short delay failures */
		if (delay_time >= 2 * HUB_SHORT_RESET_TIME)
			delay = HUB_LONG_RESET_TIME;

		dev_dbg(&hub->ports[port1 - 1]->dev,
				"not %sreset yet, waiting %dms\n",
				warm ? "warm " : "", delay);
	}

	if ((portstatus & USB_PORT_STAT_RESET))
		return -EBUSY;

	if (hub_port_warm_reset_required(hub, port1, portstatus))
		return -ENOTCONN;

	/* Device went away? */
	if (!(portstatus & USB_PORT_STAT_CONNECTION))
		return -ENOTCONN;

	/* bomb out completely if the connection bounced.  A USB 3.0
	 * connection may bounce if multiple warm resets were issued,
	 * but the device may have successfully re-connected. Ignore it.
	 */
	if (!hub_is_superspeed(hub->hdev) &&
			(portchange & USB_PORT_STAT_C_CONNECTION))
		return -ENOTCONN;

	if (!(portstatus & USB_PORT_STAT_ENABLE))
		return -EBUSY;

	if (!udev)
		return 0;

	if (hub_is_wusb(hub))
		udev->speed = USB_SPEED_WIRELESS;
	else if (hub_is_superspeed(hub->hdev))
		udev->speed = USB_SPEED_SUPER;
	else if (portstatus & USB_PORT_STAT_HIGH_SPEED)
		udev->speed = USB_SPEED_HIGH;
	else if (portstatus & USB_PORT_STAT_LOW_SPEED)
		udev->speed = USB_SPEED_LOW;
	else
		udev->speed = USB_SPEED_FULL;
	return 0;
}