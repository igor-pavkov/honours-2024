static int technisat_usb2_i2c_xfer(struct i2c_adapter *adap, struct i2c_msg *msg,
				int num)
{
	int ret = 0, i;
	struct dvb_usb_device *d = i2c_get_adapdata(adap);

	/* Ensure nobody else hits the i2c bus while we're sending our
	   sequence of messages, (such as the remote control thread) */
	if (mutex_lock_interruptible(&d->i2c_mutex) < 0)
		return -EAGAIN;

	for (i = 0; i < num; i++) {
		if (i+1 < num && msg[i+1].flags & I2C_M_RD) {
			ret = technisat_usb2_i2c_access(d->udev, msg[i+1].addr,
						msg[i].buf, msg[i].len,
						msg[i+1].buf, msg[i+1].len);
			if (ret != 0)
				break;
			i++;
		} else {
			ret = technisat_usb2_i2c_access(d->udev, msg[i].addr,
						msg[i].buf, msg[i].len,
						NULL, 0);
			if (ret != 0)
				break;
		}
	}

	if (ret == 0)
		ret = i;

	mutex_unlock(&d->i2c_mutex);

	return ret;
}