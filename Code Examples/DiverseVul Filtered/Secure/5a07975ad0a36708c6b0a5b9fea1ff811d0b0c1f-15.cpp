static int digi_startup(struct usb_serial *serial)
{
	struct device *dev = &serial->interface->dev;
	struct digi_serial *serial_priv;
	int ret;
	int i;

	/* check whether the device has the expected number of endpoints */
	if (serial->num_port_pointers < serial->type->num_ports + 1) {
		dev_err(dev, "OOB endpoints missing\n");
		return -ENODEV;
	}

	for (i = 0; i < serial->type->num_ports + 1 ; i++) {
		if (!serial->port[i]->read_urb) {
			dev_err(dev, "bulk-in endpoint missing\n");
			return -ENODEV;
		}
		if (!serial->port[i]->write_urb) {
			dev_err(dev, "bulk-out endpoint missing\n");
			return -ENODEV;
		}
	}

	serial_priv = kzalloc(sizeof(*serial_priv), GFP_KERNEL);
	if (!serial_priv)
		return -ENOMEM;

	spin_lock_init(&serial_priv->ds_serial_lock);
	serial_priv->ds_oob_port_num = serial->type->num_ports;
	serial_priv->ds_oob_port = serial->port[serial_priv->ds_oob_port_num];

	ret = digi_port_init(serial_priv->ds_oob_port,
						serial_priv->ds_oob_port_num);
	if (ret) {
		kfree(serial_priv);
		return ret;
	}

	usb_set_serial_data(serial, serial_priv);

	return 0;
}