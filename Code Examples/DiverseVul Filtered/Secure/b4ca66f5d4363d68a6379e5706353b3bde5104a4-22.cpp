static size_t consume_u7_r(RBuffer *b, ut64 bound, ut8 *out) {
	size_t n = 0;
	ut32 tmp = consume_r (b, bound, &n, read_u32_leb128);
	if (out) {
		*out = (ut8) (tmp & 0x7f);
	}
	return n;
}