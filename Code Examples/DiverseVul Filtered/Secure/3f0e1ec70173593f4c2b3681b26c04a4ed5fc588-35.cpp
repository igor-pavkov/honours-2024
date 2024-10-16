static inline __maybe_unused uint32_t h2_get_n32(const struct buffer *b, int o)
{
	return readv_n32(b_ptr(b, o), b_end(b) - b_ptr(b, o), b->data);
}