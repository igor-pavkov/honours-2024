static int rx_alloc_submit(struct usbnet *dev, gfp_t flags)
{
	struct urb	*urb;
	int		i;
	int		ret = 0;

	/* don't refill the queue all at once */
	for (i = 0; i < 10 && dev->rxq.qlen < RX_QLEN(dev); i++) {
		urb = usb_alloc_urb(0, flags);
		if (urb != NULL) {
			ret = rx_submit(dev, urb, flags);
			if (ret)
				goto err;
		} else {
			ret = -ENOMEM;
			goto err;
		}
	}
err:
	return ret;
}