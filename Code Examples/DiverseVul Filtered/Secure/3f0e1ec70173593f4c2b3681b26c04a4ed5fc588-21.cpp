static inline __maybe_unused uint16_t h2_get_n16(const struct buffer *b, int o)
{
	return readv_n16(b_ptr(b, o), b_end(b) - b_ptr(b, o), b->data);
}