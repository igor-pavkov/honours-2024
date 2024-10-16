static double php_ifd_get_double(char *data) {
	/* Copy to avoid alignment issues */
	double f;
	memcpy(&f, data, sizeof(double));
	return f;
}