static void ati_remote2_urb_cleanup(struct ati_remote2 *ar2)
{
	int i;

	for (i = 0; i < 2; i++) {
		usb_free_urb(ar2->urb[i]);
		usb_free_coherent(ar2->udev, 4, ar2->buf[i], ar2->buf_dma[i]);
	}
}