static int r_egg_append_bytes(REgg *egg, const ut8 *b, int len) {
	if (!r_egg_raw (egg, b, len)) {
		return false;
	}

	if (!r_buf_append_bytes (egg->bin, b, len)) {
		return false;
	}

	return true;
}