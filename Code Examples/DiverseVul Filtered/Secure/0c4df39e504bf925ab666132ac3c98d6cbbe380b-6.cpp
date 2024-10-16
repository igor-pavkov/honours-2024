static int technisat_usb2_eeprom_lrc_read(struct dvb_usb_device *d,
	u16 offset, u8 *b, u16 length, u8 tries)
{
	u8 bo = offset & 0xff;
	struct i2c_msg msg[] = {
		{
			.addr = 0x50 | ((offset >> 8) & 0x3),
			.buf = &bo,
			.len = 1
		}, {
			.addr = 0x50 | ((offset >> 8) & 0x3),
			.flags	= I2C_M_RD,
			.buf = b,
			.len = length
		}
	};

	while (tries--) {
		int status;

		if (i2c_transfer(&d->i2c_adap, msg, 2) != 2)
			break;

		status =
			technisat_usb2_calc_lrc(b, length - 1) == b[length - 1];

		if (status)
			return 0;
	}

	return -EREMOTEIO;
}