static int sisusb_get_sdram_size(struct sisusb_usb_data *sisusb, int *iret,
		int bw, int chab)
{
	int ret = 0, i2ret = 0, i, j;
	static const u8 sdramtype[13][5] = {
		{ 2, 12, 9, 64, 0x35 },
		{ 1, 13, 9, 64, 0x44 },
		{ 2, 12, 8, 32, 0x31 },
		{ 2, 11, 9, 32, 0x25 },
		{ 1, 12, 9, 32, 0x34 },
		{ 1, 13, 8, 32, 0x40 },
		{ 2, 11, 8, 16, 0x21 },
		{ 1, 12, 8, 16, 0x30 },
		{ 1, 11, 9, 16, 0x24 },
		{ 1, 11, 8,  8, 0x20 },
		{ 2,  9, 8,  4, 0x01 },
		{ 1, 10, 8,  4, 0x10 },
		{ 1,  9, 8,  2, 0x00 }
	};

	*iret = 1; /* error */

	for (i = 0; i < 13; i++) {
		ret |= SETIREGANDOR(SISSR, 0x13, 0x80, sdramtype[i][4]);
		for (j = 2; j > 0; j--) {
			ret |= sisusb_set_rank(sisusb, &i2ret, i, j, chab,
					sdramtype, bw);
			if (!i2ret)
				continue;

			ret |= sisusb_check_ranks(sisusb, &i2ret, j, i, bw,
					sdramtype);
			if (i2ret) {
				*iret = 0;	/* ram size found */
				return ret;
			}
		}
	}

	return ret;
}