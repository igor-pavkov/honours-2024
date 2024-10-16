static __maybe_unused int h2_peek_frame_hdr(const struct buffer *b, struct h2_fh *h)
{
	uint64_t w;

	if (b->i < 9)
		return 0;

	w = readv_n64(b_ptr(b,1), b_end(b) - b_ptr(b,1), b->data);
	h->len = *b->p << 16;
	h->sid = w & 0x7FFFFFFF; /* RFC7540#4.1: R bit must be ignored */
	h->ff = w >> 32;
	h->ft = w >> 40;
	h->len += w >> 48;
	return 1;
}