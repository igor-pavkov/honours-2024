static int http_stream_read(
	git_smart_subtransport_stream *stream,
	char *buffer,
	size_t buf_size,
	size_t *bytes_read)
{
	http_stream *s = (http_stream *)stream;
	http_subtransport *t = OWNING_SUBTRANSPORT(s);
	parser_context ctx;
	size_t bytes_parsed;

replay:
	*bytes_read = 0;

	assert(t->connected);

	if (!s->sent_request) {
		git_buf request = GIT_BUF_INIT;

		clear_parser_state(t);

		if (gen_request(&request, s, 0) < 0)
			return -1;

		if (git_stream_write(t->io, request.ptr, request.size, 0) < 0) {
			git_buf_free(&request);
			return -1;
		}

		git_buf_free(&request);

		s->sent_request = 1;
	}

	if (!s->received_response) {
		if (s->chunked) {
			assert(s->verb == post_verb);

			/* Flush, if necessary */
			if (s->chunk_buffer_len > 0 &&
				write_chunk(t->io, s->chunk_buffer, s->chunk_buffer_len) < 0)
				return -1;

			s->chunk_buffer_len = 0;

			/* Write the final chunk. */
			if (git_stream_write(t->io, "0\r\n\r\n", 5, 0) < 0)
				return -1;
		}

		s->received_response = 1;
	}

	while (!*bytes_read && !t->parse_finished) {
		size_t data_offset;
		int error;

		/*
		 * Make the parse_buffer think it's as full of data as
		 * the buffer, so it won't try to recv more data than
		 * we can put into it.
		 *
		 * data_offset is the actual data offset from which we
		 * should tell the parser to start reading.
		 */
		if (buf_size >= t->parse_buffer.len) {
			t->parse_buffer.offset = 0;
		} else {
			t->parse_buffer.offset = t->parse_buffer.len - buf_size;
		}

		data_offset = t->parse_buffer.offset;

		if (gitno_recv(&t->parse_buffer) < 0)
			return -1;

		/* This call to http_parser_execute will result in invocations of the
		 * on_* family of callbacks. The most interesting of these is
		 * on_body_fill_buffer, which is called when data is ready to be copied
		 * into the target buffer. We need to marshal the buffer, buf_size, and
		 * bytes_read parameters to this callback. */
		ctx.t = t;
		ctx.s = s;
		ctx.buffer = buffer;
		ctx.buf_size = buf_size;
		ctx.bytes_read = bytes_read;

		/* Set the context, call the parser, then unset the context. */
		t->parser.data = &ctx;

		bytes_parsed = http_parser_execute(&t->parser,
			&t->settings,
			t->parse_buffer.data + data_offset,
			t->parse_buffer.offset - data_offset);

		t->parser.data = NULL;

		/* If there was a handled authentication failure, then parse_error
		 * will have signaled us that we should replay the request. */
		if (PARSE_ERROR_REPLAY == t->parse_error) {
			s->sent_request = 0;

			if ((error = http_connect(t)) < 0)
				return error;

			goto replay;
		}

		if (t->parse_error == PARSE_ERROR_EXT) {
			return t->error;
		}

		if (t->parse_error < 0)
			return -1;

		if (bytes_parsed != t->parse_buffer.offset - data_offset) {
			giterr_set(GITERR_NET,
				"HTTP parser error: %s",
				http_errno_description((enum http_errno)t->parser.http_errno));
			return -1;
		}
	}

	return 0;
}