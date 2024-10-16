void sisusb_delete(struct kref *kref)
{
	struct sisusb_usb_data *sisusb = to_sisusb_dev(kref);

	if (!sisusb)
		return;

	usb_put_dev(sisusb->sisusb_dev);

	sisusb->sisusb_dev = NULL;
	sisusb_free_buffers(sisusb);
	sisusb_free_urbs(sisusb);
#ifdef CONFIG_USB_SISUSBVGA_CON
	kfree(sisusb->SiS_Pr);
#endif
	kfree(sisusb);
}