static void technisat_usb2_eeprom_dump(struct dvb_usb_device *d)
{
	u8 reg;
	u8 b[16];
	int i, j;

	/* full EEPROM dump */
	for (j = 0; j < 256 * 4; j += 16) {
		reg = j;
		if (technisat_usb2_i2c_access(d->udev, 0x50 + j / 256, &reg, 1, b, 16) != 0)
			break;

		deb_eeprom("EEPROM: %01x%02x: ", j / 256, reg);
		for (i = 0; i < 16; i++)
			deb_eeprom("%02x ", b[i]);
		deb_eeprom("\n");
	}
}