int usb_hub_claim_port(struct usb_device *hdev, unsigned port1,
		       struct usb_dev_state *owner)
{
	int rc;
	struct usb_dev_state **powner;

	rc = find_port_owner(hdev, port1, &powner);
	if (rc)
		return rc;
	if (*powner)
		return -EBUSY;
	*powner = owner;
	return rc;
}