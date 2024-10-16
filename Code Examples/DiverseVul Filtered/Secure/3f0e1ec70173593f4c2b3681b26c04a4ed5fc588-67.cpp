static inline __maybe_unused void h2_get_buf_bytes(void *dst, size_t bytes,
                                    const struct buffer *b, int o)
{
	readv_bytes(dst, bytes, b_ptr(b, o), b_end(b) - b_ptr(b, o), b->data);
}