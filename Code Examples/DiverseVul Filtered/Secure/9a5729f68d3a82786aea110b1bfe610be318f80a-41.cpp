static void sisusb_disconnect(struct usb_interface *intf)
{
	struct sisusb_usb_data *sisusb;

	/* This should *not* happen */
	sisusb = usb_get_intfdata(intf);
	if (!sisusb)
		return;

#ifdef CONFIG_USB_SISUSBVGA_CON
	sisusb_console_exit(sisusb);
#endif

	usb_deregister_dev(intf, &usb_sisusb_class);

	mutex_lock(&sisusb->lock);

	/* Wait for all URBs to complete and kill them in case (MUST do) */
	if (!sisusb_wait_all_out_complete(sisusb))
		sisusb_kill_all_busy(sisusb);

	usb_set_intfdata(intf, NULL);

	sisusb->present = 0;
	sisusb->ready = 0;

	mutex_unlock(&sisusb->lock);

	/* decrement our usage count */
	kref_put(&sisusb->kref, sisusb_delete);
}