static size_t consume_s7_r(RBuffer *b, ut64 bound, st8 *out) {
	size_t n = 0;
	ut32 tmp = consume_r (b, bound, &n, (ConsumeFcn)read_i32_leb128);
	if (out) {
		*out = (st8) (((tmp & 0x10000000) << 7) | (tmp & 0x7f));
	}
	return n;
}