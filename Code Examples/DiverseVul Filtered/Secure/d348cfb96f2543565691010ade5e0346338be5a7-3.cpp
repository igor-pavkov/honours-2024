static float php_ifd_get_float(char *data) {
	/* Copy to avoid alignment issues */
	float f;
	memcpy(&f, data, sizeof(float));
	return f;
}