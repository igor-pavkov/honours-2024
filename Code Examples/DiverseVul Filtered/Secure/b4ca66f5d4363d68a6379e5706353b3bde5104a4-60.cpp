static size_t consume_u32_r(RBuffer *b, ut64 bound, ut32 *out) {
	size_t n = 0;
	ut32 tmp = consume_r (b, bound, &n, read_u32_leb128);
	if (out) {
		*out = tmp;
	}
	return n;
}