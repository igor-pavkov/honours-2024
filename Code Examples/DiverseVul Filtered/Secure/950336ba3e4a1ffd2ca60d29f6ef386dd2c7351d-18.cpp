static int ati_remote2_urb_init(struct ati_remote2 *ar2)
{
	struct usb_device *udev = ar2->udev;
	int i, pipe, maxp;

	for (i = 0; i < 2; i++) {
		ar2->buf[i] = usb_alloc_coherent(udev, 4, GFP_KERNEL, &ar2->buf_dma[i]);
		if (!ar2->buf[i])
			return -ENOMEM;

		ar2->urb[i] = usb_alloc_urb(0, GFP_KERNEL);
		if (!ar2->urb[i])
			return -ENOMEM;

		pipe = usb_rcvintpipe(udev, ar2->ep[i]->bEndpointAddress);
		maxp = usb_maxpacket(udev, pipe, usb_pipeout(pipe));
		maxp = maxp > 4 ? 4 : maxp;

		usb_fill_int_urb(ar2->urb[i], udev, pipe, ar2->buf[i], maxp,
				 i ? ati_remote2_complete_key : ati_remote2_complete_mouse,
				 ar2, ar2->ep[i]->bInterval);
		ar2->urb[i]->transfer_dma = ar2->buf_dma[i];
		ar2->urb[i]->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
	}

	return 0;
}