static int technisat_usb2_read_mac_address(struct dvb_usb_device *d,
		u8 mac[])
{
	u8 buf[EEPROM_MAC_TOTAL];

	if (technisat_usb2_eeprom_lrc_read(d, EEPROM_MAC_START,
				buf, EEPROM_MAC_TOTAL, 4) != 0)
		return -ENODEV;

	memcpy(mac, buf, 6);
	return 0;
}