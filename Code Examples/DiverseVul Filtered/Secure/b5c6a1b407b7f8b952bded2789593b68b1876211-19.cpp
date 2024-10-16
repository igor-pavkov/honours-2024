static int on_body_fill_buffer(http_parser *parser, const char *str, size_t len)
{
	parser_context *ctx = (parser_context *) parser->data;
	http_subtransport *t = ctx->t;

	/* If our goal is to replay the request (either an auth failure or
	 * a redirect) then don't bother buffering since we're ignoring the
	 * content anyway.
	 */
	if (t->parse_error == PARSE_ERROR_REPLAY)
		return 0;

	if (ctx->buf_size < len) {
		giterr_set(GITERR_NET, "Can't fit data in the buffer");
		return t->parse_error = PARSE_ERROR_GENERIC;
	}

	memcpy(ctx->buffer, str, len);
	*(ctx->bytes_read) += len;
	ctx->buffer += len;
	ctx->buf_size -= len;

	return 0;
}