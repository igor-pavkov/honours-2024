static void __handle_set_rx_mode(struct usbnet *dev)
{
	if (dev->driver_info->set_rx_mode)
		(dev->driver_info->set_rx_mode)(dev);

	clear_bit(EVENT_SET_RX_MODE, &dev->flags);
}