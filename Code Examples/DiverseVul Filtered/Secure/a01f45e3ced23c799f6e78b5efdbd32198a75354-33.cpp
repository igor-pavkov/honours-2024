static inline __maybe_unused void h2_skip_frame_hdr(struct buffer *b)
{
	b_del(b, 9);
}