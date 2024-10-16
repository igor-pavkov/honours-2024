void usbnet_update_max_qlen(struct usbnet *dev)
{
	enum usb_device_speed speed = dev->udev->speed;

	switch (speed) {
	case USB_SPEED_HIGH:
		dev->rx_qlen = MAX_QUEUE_MEMORY / dev->rx_urb_size;
		dev->tx_qlen = MAX_QUEUE_MEMORY / dev->hard_mtu;
		break;
	case USB_SPEED_SUPER:
		/*
		 * Not take default 5ms qlen for super speed HC to
		 * save memory, and iperf tests show 2.5ms qlen can
		 * work well
		 */
		dev->rx_qlen = 5 * MAX_QUEUE_MEMORY / dev->rx_urb_size;
		dev->tx_qlen = 5 * MAX_QUEUE_MEMORY / dev->hard_mtu;
		break;
	default:
		dev->rx_qlen = dev->tx_qlen = 4;
	}
}