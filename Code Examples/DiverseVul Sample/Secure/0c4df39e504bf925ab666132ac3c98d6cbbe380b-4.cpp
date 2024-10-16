static u8 technisat_usb2_calc_lrc(const u8 *b, u16 length)
{
	u8 lrc = 0;
	while (--length)
		lrc ^= *b++;
	return lrc;
}