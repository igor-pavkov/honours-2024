static __maybe_unused int h2_peek_frame_hdr(const struct buffer *b, int o, struct h2_fh *h)
{
	uint64_t w;

	if (b_data(b) < o + 9)
		return 0;

	w = h2_get_n64(b, o + 1);
	h->len = *(uint8_t*)b_peek(b, o) << 16;
	h->sid = w & 0x7FFFFFFF; /* RFC7540#4.1: R bit must be ignored */
	h->ff = w >> 32;
	h->ft = w >> 40;
	h->len += w >> 48;
	return 1;
}