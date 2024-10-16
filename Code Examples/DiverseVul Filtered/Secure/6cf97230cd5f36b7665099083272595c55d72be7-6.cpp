static struct dvb_usb_device_description *dvb_usb_find_device(struct usb_device *udev, struct dvb_usb_device_properties *props, int *cold)
{
	int i, j;
	struct dvb_usb_device_description *desc = NULL;

	*cold = -1;

	for (i = 0; i < props->num_device_descs; i++) {

		for (j = 0; j < DVB_USB_ID_MAX_NUM && props->devices[i].cold_ids[j] != NULL; j++) {
			deb_info("check for cold %x %x\n", props->devices[i].cold_ids[j]->idVendor, props->devices[i].cold_ids[j]->idProduct);
			if (props->devices[i].cold_ids[j]->idVendor  == le16_to_cpu(udev->descriptor.idVendor) &&
				props->devices[i].cold_ids[j]->idProduct == le16_to_cpu(udev->descriptor.idProduct)) {
				*cold = 1;
				desc = &props->devices[i];
				break;
			}
		}

		if (desc != NULL)
			break;

		for (j = 0; j < DVB_USB_ID_MAX_NUM && props->devices[i].warm_ids[j] != NULL; j++) {
			deb_info("check for warm %x %x\n", props->devices[i].warm_ids[j]->idVendor, props->devices[i].warm_ids[j]->idProduct);
			if (props->devices[i].warm_ids[j]->idVendor == le16_to_cpu(udev->descriptor.idVendor) &&
				props->devices[i].warm_ids[j]->idProduct == le16_to_cpu(udev->descriptor.idProduct)) {
				*cold = 0;
				desc = &props->devices[i];
				break;
			}
		}
	}

	if (desc != NULL && props->identify_state != NULL)
		props->identify_state(udev, props, &desc, cold);

	return desc;
}