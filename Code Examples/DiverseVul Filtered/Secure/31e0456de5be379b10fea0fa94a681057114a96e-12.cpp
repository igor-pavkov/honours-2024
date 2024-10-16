static void smsusb_disconnect(struct usb_interface *intf)
{
	smsusb_term_device(intf);
}