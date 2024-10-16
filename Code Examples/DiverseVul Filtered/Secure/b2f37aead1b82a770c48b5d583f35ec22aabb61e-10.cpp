static int kiss_esc_crc(unsigned char *s, unsigned char *d, unsigned short crc,
	int len)
{
	unsigned char *ptr = d;
	unsigned char c=0;

	*ptr++ = END;
	while (len > 0) {
		if (len > 2)
			c = *s++;
		else if (len > 1)
			c = crc >> 8;
		else
			c = crc & 0xff;

		len--;

		switch (c) {
		case END:
			*ptr++ = ESC;
			*ptr++ = ESC_END;
			break;
		case ESC:
			*ptr++ = ESC;
			*ptr++ = ESC_ESC;
			break;
		default:
			*ptr++ = c;
			break;
		}
	}
	*ptr++ = END;

	return ptr - d;
}