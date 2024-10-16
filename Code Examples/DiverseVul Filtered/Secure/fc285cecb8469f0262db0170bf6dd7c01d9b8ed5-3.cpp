static bool check_buffer(RBinFile *bf, RBuffer *b) {
	if (r_buf_size (b) >= 0x20) {
		ut8 magic[4] = {0};
		if (r_buf_read_at (b, 0, magic, sizeof (magic)) != 4) {
			return false;
		}
		return !memcmp (magic, "XALZ", 4);
	}
	return false;
}