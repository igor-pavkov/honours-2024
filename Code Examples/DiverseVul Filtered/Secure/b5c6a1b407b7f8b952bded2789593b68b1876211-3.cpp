static int write_chunk(git_stream *io, const char *buffer, size_t len)
{
	git_buf buf = GIT_BUF_INIT;

	/* Chunk header */
	git_buf_printf(&buf, "%" PRIxZ "\r\n", len);

	if (git_buf_oom(&buf))
		return -1;

	if (git_stream_write(io, buf.ptr, buf.size, 0) < 0) {
		git_buf_free(&buf);
		return -1;
	}

	git_buf_free(&buf);

	/* Chunk body */
	if (len > 0 && git_stream_write(io, buffer, len, 0) < 0)
		return -1;

	/* Chunk footer */
	if (git_stream_write(io, "\r\n", 2, 0) < 0)
		return -1;

	return 0;
}