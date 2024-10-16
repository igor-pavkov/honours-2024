static size_t consume_limits_r(RBuffer *b, ut64 bound, struct r_bin_wasm_resizable_limits_t *out) {
	r_return_val_if_fail (b && out, 0);
	if (bound >= r_buf_size (b) || r_buf_tell (b) > bound || !out) {
		return 0;
	}
	ut32 i = r_buf_tell (b);
	if (!consume_u7_r (b, bound, &out->flags)) {
		return 0;
	}
	if (!consume_u32_r (b, bound, &out->initial)) {
		return 0;
	}
	if (out->flags && !consume_u32_r (b, bound, &out->maximum)) {
		return 0;
	}
	int delta = r_buf_tell (b) - i;
	return (delta > 0)? delta: 0;
}