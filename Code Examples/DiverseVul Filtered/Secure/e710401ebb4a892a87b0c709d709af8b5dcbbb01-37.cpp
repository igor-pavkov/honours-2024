R_API int r_egg_raw(REgg *egg, const ut8 *b, int len) {
	int outlen = len * 2; // two hexadecimal digits per byte
	char *out = malloc (outlen + 1);
	if (!out) {
		return false;
	}
	(void)r_hex_bin2str (b, len, out);
	r_buf_append_bytes (egg->buf, (const ut8 *)".hex ", 5);
	r_buf_append_bytes (egg->buf, (const ut8 *)out, outlen);
	r_buf_append_bytes (egg->buf, (const ut8 *)"\n", 1);
	free (out);
	return true;
}