int dvb_usb_device_power_ctrl(struct dvb_usb_device *d, int onoff)
{
	if (onoff)
		d->powered++;
	else
		d->powered--;

	if (d->powered == 0 || (onoff && d->powered == 1)) { /* when switching from 1 to 0 or from 0 to 1 */
		deb_info("power control: %d\n", onoff);
		if (d->props.power_ctrl)
			return d->props.power_ctrl(d, onoff);
	}
	return 0;
}