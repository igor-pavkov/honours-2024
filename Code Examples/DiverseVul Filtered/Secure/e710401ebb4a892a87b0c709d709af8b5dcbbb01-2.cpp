static int r_egg_prepend_bytes(REgg *egg, const ut8 *b, int len) {
	if (!r_egg_raw_prepend (egg, b, len)) {
		return false;
	}
	if (!r_buf_prepend_bytes (egg->bin, b, len)) {
		return false;
	}
	return true;
}