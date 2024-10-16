static bool check_buffer(RBinFile *bf, RBuffer *b) {
	ut8 buf[4];
	r_buf_read_at (b, 0, buf, sizeof (buf));
	return !memcmp (buf, "\x02\xff\x01\xff", 4);
}