int usb_remove_device(struct usb_device *udev)
{
	struct usb_hub *hub;
	struct usb_interface *intf;

	if (!udev->parent)	/* Can't remove a root hub */
		return -EINVAL;
	hub = usb_hub_to_struct_hub(udev->parent);
	intf = to_usb_interface(hub->intfdev);

	usb_autopm_get_interface(intf);
	set_bit(udev->portnum, hub->removed_bits);
	hub_port_logical_disconnect(hub, udev->portnum);
	usb_autopm_put_interface(intf);
	return 0;
}