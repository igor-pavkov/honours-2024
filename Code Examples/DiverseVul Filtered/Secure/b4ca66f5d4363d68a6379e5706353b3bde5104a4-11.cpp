static size_t consume_init_expr_r(RBuffer *b, ut64 bound, ut8 eoc, void *out) {
	if (!b || bound >= r_buf_size (b) || r_buf_tell (b) > bound) {
		return 0;
	}
	size_t res = 0;
	ut8 cur = r_buf_read8 (b);
	while (r_buf_tell (b) <= bound && cur != eoc) {
		cur = r_buf_read8 (b);
		res++;
	}
	if (cur != eoc) {
		return 0;
	}
	return res + 1;
}