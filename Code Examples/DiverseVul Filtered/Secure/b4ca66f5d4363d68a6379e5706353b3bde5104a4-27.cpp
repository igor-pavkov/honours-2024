static ut32 consume_r(RBuffer *b, ut64 bound, size_t *n_out, ConsumeFcn consume_fcn) {
	r_return_val_if_fail (b && n_out && consume_fcn, 0);

	ut32 tmp;
	ut64 cur = r_buf_tell (b);
	if (bound >= r_buf_size (b) || cur > bound) {
		return 0;
	}
	// 16 bytes are enough to store 128bits values
	ut8 *buf = R_NEWS (ut8, 16);
	if (!buf) {
		return 0;
	}
	r_buf_read (b, buf, 16);
	size_t n = consume_fcn (buf, buf + bound + 1, &tmp);
	if (!n) {
		free (buf);
		return 0;
	}
	r_buf_seek (b, cur + n, R_BUF_SET);
	*n_out = n;
	free (buf);
	return tmp;
}