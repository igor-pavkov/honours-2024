static int edge_calc_num_ports(struct usb_serial *serial,
				struct usb_serial_endpoints *epds)
{
	struct device *dev = &serial->interface->dev;
	unsigned char num_ports = serial->type->num_ports;

	/* Make sure we have the required endpoints when in download mode. */
	if (serial->interface->cur_altsetting->desc.bNumEndpoints > 1) {
		if (epds->num_bulk_in < num_ports ||
				epds->num_bulk_out < num_ports ||
				epds->num_interrupt_in < 1) {
			dev_err(dev, "required endpoints missing\n");
			return -ENODEV;
		}
	}

	return num_ports;
}