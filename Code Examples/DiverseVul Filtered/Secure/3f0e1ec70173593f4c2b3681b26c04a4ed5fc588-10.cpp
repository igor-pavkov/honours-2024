static inline __maybe_unused uint64_t h2_get_n64(const struct buffer *b, int o)
{
	return readv_n64(b_ptr(b, o), b_end(b) - b_ptr(b, o), b->data);
}