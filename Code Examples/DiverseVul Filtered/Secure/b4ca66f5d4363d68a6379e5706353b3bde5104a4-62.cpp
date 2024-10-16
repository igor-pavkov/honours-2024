static bool consume_str_r(RBuffer *b, ut64 bound, size_t len, char *out) {
	r_return_val_if_fail (b && bound > 0 && bound < r_buf_size (b), 0);
	*out = 0;

	if (r_buf_tell (b) + len <= bound + 1) {
		if (r_buf_read (b, (ut8 *)out, len) == len) {
			out[len] = 0;
			return true;
		}
	}
	return false;
}