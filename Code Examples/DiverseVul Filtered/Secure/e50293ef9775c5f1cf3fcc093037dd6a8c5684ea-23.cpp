static unsigned hub_is_wusb(struct usb_hub *hub)
{
	struct usb_hcd *hcd;
	if (hub->hdev->parent != NULL)  /* not a root hub? */
		return 0;
	hcd = container_of(hub->hdev->bus, struct usb_hcd, self);
	return hcd->wireless;
}