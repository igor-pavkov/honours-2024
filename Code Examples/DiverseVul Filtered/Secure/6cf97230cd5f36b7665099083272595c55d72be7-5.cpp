static int dvb_usb_init(struct dvb_usb_device *d, short *adapter_nums)
{
	int ret = 0;

	mutex_init(&d->data_mutex);
	mutex_init(&d->usb_mutex);
	mutex_init(&d->i2c_mutex);

	d->state = DVB_USB_STATE_INIT;

	if (d->props.size_of_priv > 0) {
		d->priv = kzalloc(d->props.size_of_priv, GFP_KERNEL);
		if (d->priv == NULL) {
			err("no memory for priv in 'struct dvb_usb_device'");
			return -ENOMEM;
		}
	}

	/* check the capabilities and set appropriate variables */
	dvb_usb_device_power_ctrl(d, 1);

	if ((ret = dvb_usb_i2c_init(d)) ||
		(ret = dvb_usb_adapter_init(d, adapter_nums))) {
		dvb_usb_exit(d);
		return ret;
	}

	if ((ret = dvb_usb_remote_init(d)))
		err("could not initialize remote control.");

	dvb_usb_device_power_ctrl(d, 0);

	return 0;
}