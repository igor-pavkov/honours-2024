static int sisusb_check_ranks(struct sisusb_usb_data *sisusb,
		int *iret, int rankno, int idx, int bw, const u8 rtype[][5])
{
	int ret = 0, i, i2ret;
	u32 inc;

	*iret = 0;

	for (i = rankno; i >= 1; i--) {
		inc = 1 << (rtype[idx][2] + rtype[idx][1] + rtype[idx][0] +
				bw / 64 + i);
		ret |= sisusb_check_rbc(sisusb, &i2ret, inc, 2);
		if (!i2ret)
			return ret;
	}

	inc = 1 << (rtype[idx][2] + bw / 64 + 2);
	ret |= sisusb_check_rbc(sisusb, &i2ret, inc, 4);
	if (!i2ret)
		return ret;

	inc = 1 << (10 + bw / 64);
	ret |= sisusb_check_rbc(sisusb, &i2ret, inc, 2);
	if (!i2ret)
		return ret;

	*iret = 1;
	return ret;
}