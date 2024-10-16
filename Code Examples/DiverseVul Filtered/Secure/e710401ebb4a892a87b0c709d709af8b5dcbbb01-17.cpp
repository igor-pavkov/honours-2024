static int r_egg_raw_prepend(REgg *egg, const ut8 *b, int len) {
	int outlen = len * 2; // two hexadecimal digits per byte
	char *out = malloc (outlen + 1);
	if (!out) {
		return false;
	}
	r_hex_bin2str (b, len, out);
	r_buf_prepend_bytes (egg->buf, (const ut8 *)"\n", 1);
	r_buf_prepend_bytes (egg->buf, (const ut8 *)out, outlen);
	r_buf_prepend_bytes (egg->buf, (const ut8 *)".hex ", 5);
	free (out);
	return true;
}