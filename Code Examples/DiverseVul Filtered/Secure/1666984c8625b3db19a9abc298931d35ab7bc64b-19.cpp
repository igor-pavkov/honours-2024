static void intr_complete (struct urb *urb)
{
	struct usbnet	*dev = urb->context;
	int		status = urb->status;

	switch (status) {
	/* success */
	case 0:
		dev->driver_info->status(dev, urb);
		break;

	/* software-driven interface shutdown */
	case -ENOENT:		/* urb killed */
	case -ESHUTDOWN:	/* hardware gone */
		netif_dbg(dev, ifdown, dev->net,
			  "intr shutdown, code %d\n", status);
		return;

	/* NOTE:  not throttling like RX/TX, since this endpoint
	 * already polls infrequently
	 */
	default:
		netdev_dbg(dev->net, "intr status %d\n", status);
		break;
	}

	status = usb_submit_urb (urb, GFP_ATOMIC);
	if (status != 0)
		netif_err(dev, timer, dev->net,
			  "intr resubmit --> %d\n", status);
}