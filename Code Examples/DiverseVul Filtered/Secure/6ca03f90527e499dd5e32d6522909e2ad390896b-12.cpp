static void to_utf8(struct vc_data *vc, uint c)
{
	if (c < 0x80)
		/*  0******* */
		put_queue(vc, c);
	else if (c < 0x800) {
		/* 110***** 10****** */
		put_queue(vc, 0xc0 | (c >> 6));
		put_queue(vc, 0x80 | (c & 0x3f));
	} else if (c < 0x10000) {
		if (c >= 0xD800 && c < 0xE000)
			return;
		if (c == 0xFFFF)
			return;
		/* 1110**** 10****** 10****** */
		put_queue(vc, 0xe0 | (c >> 12));
		put_queue(vc, 0x80 | ((c >> 6) & 0x3f));
		put_queue(vc, 0x80 | (c & 0x3f));
	} else if (c < 0x110000) {
		/* 11110*** 10****** 10****** 10****** */
		put_queue(vc, 0xf0 | (c >> 18));
		put_queue(vc, 0x80 | ((c >> 12) & 0x3f));
		put_queue(vc, 0x80 | ((c >> 6) & 0x3f));
		put_queue(vc, 0x80 | (c & 0x3f));
	}
}