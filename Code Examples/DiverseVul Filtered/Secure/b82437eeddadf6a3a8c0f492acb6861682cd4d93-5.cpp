static void php_ifd_set32u(char *data, size_t value, int motorola_intel)
{
	if (motorola_intel) {
		data[0] = (value & 0xFF000000) >> 24;
		data[1] = (char) ((value & 0x00FF0000) >> 16);
		data[2] = (value & 0x0000FF00) >>  8;
		data[3] = (value & 0x000000FF);
	} else {
		data[3] = (value & 0xFF000000) >> 24;
		data[2] = (char) ((value & 0x00FF0000) >> 16);
		data[1] = (value & 0x0000FF00) >>  8;
		data[0] = (value & 0x000000FF);
	}
}