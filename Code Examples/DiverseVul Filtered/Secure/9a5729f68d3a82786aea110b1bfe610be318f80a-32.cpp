static void sisusb_free_buffers(struct sisusb_usb_data *sisusb)
{
	int i;

	for (i = 0; i < NUMOBUFS; i++) {
		kfree(sisusb->obuf[i]);
		sisusb->obuf[i] = NULL;
	}
	kfree(sisusb->ibuf);
	sisusb->ibuf = NULL;
}