static int sisusb_set_rank(struct sisusb_usb_data *sisusb, int *iret,
		int index, u8 rankno, u8 chab, const u8 dramtype[][5], int bw)
{
	int ret = 0, ranksize;
	u8 tmp;

	*iret = 0;

	if ((rankno == 2) && (dramtype[index][0] == 2))
		return ret;

	ranksize = dramtype[index][3] / 2 * bw / 32;

	if ((ranksize * rankno) > 128)
		return ret;

	tmp = 0;
	while ((ranksize >>= 1) > 0)
		tmp += 0x10;

	tmp |= ((rankno - 1) << 2);
	tmp |= ((bw / 64) & 0x02);
	tmp |= (chab & 0x01);

	ret = SETIREG(SISSR, 0x14, tmp);
	ret |= sisusb_triggersr16(sisusb, 0); /* sic! */

	*iret = 1;

	return ret;
}