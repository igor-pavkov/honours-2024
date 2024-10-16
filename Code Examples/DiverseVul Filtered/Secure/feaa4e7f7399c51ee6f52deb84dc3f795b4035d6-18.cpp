static bool check_buffer(RBinFile *bf, RBuffer *b) {
	if (r_buf_size (b) > 4) {
		ut8 buf[4];
		r_buf_read_at (b, 0, buf, sizeof (buf));
		if (!memcmp (buf, "\xcf\xfa\xed\xfe", 4)) {
			return is_kernelcache_buffer (b);
		}
	}
	return false;
}