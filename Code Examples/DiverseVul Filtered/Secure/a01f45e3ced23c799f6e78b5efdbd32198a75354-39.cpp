static inline __maybe_unused int h2_get_frame_hdr(struct buffer *b, struct h2_fh *h)
{
	int ret;

	ret = h2_peek_frame_hdr(b, 0, h);
	if (ret > 0)
		h2_skip_frame_hdr(b);
	return ret;
}