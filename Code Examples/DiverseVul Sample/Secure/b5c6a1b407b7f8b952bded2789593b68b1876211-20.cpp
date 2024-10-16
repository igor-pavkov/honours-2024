static int http_stream_write_single(
	git_smart_subtransport_stream *stream,
	const char *buffer,
	size_t len)
{
	http_stream *s = (http_stream *)stream;
	http_subtransport *t = OWNING_SUBTRANSPORT(s);
	git_buf request = GIT_BUF_INIT;

	assert(t->connected);

	if (s->sent_request) {
		giterr_set(GITERR_NET, "Subtransport configured for only one write");
		return -1;
	}

	clear_parser_state(t);

	if (gen_request(&request, s, len) < 0)
		return -1;

	if (git_stream_write(t->io, request.ptr, request.size, 0) < 0)
		goto on_error;

	if (len && git_stream_write(t->io, buffer, len, 0) < 0)
		goto on_error;

	git_buf_free(&request);
	s->sent_request = 1;

	return 0;

on_error:
	git_buf_free(&request);
	return -1;
}