int message_snippet_generate(struct istream *input,
			     unsigned int max_snippet_chars,
			     string_t *snippet)
{
	struct message_parser_ctx *parser;
	struct message_part *parts;
	struct message_decoder_context *decoder;
	struct message_block raw_block, block;
	struct snippet_context ctx;
	pool_t pool;
	int ret;

	i_zero(&ctx);
	pool = pool_alloconly_create("message snippet", 2048);
	ctx.snippet.snippet = str_new(pool, max_snippet_chars);
	ctx.snippet.chars_left = max_snippet_chars;
	ctx.quoted_snippet.snippet = str_new(pool, max_snippet_chars);
	ctx.quoted_snippet.chars_left = max_snippet_chars;
	parser = message_parser_init(pool_datastack_create(), input, 0, 0);
	decoder = message_decoder_init(NULL, 0);
	while ((ret = message_parser_parse_next_block(parser, &raw_block)) > 0) {
		if (!message_decoder_decode_next_block(decoder, &raw_block, &block))
			continue;
		if (block.size == 0) {
			const char *ct;

			if (block.hdr != NULL)
				continue;

			/* end of headers - verify that we can use this
			   Content-Type. we get here only once, because we
			   always handle only one non-multipart MIME part. */
			ct = message_decoder_current_content_type(decoder);
			if (ct == NULL)
				/* text/plain */ ;
			else if (mail_html2text_content_type_match(ct)) {
				ctx.html2text = mail_html2text_init(0);
				ctx.plain_output = buffer_create_dynamic(pool, 1024);
			} else if (strncasecmp(ct, "text/", 5) != 0)
				break;
			continue;
		}
		if (!snippet_generate(&ctx, block.data, block.size))
			break;
	}
	i_assert(ret != 0);
	message_decoder_deinit(&decoder);
	message_parser_deinit(&parser, &parts);
	mail_html2text_deinit(&ctx.html2text);
	if (ctx.snippet.snippet->used != 0)
		snippet_copy(str_c(ctx.snippet.snippet), snippet);
	else if (ctx.quoted_snippet.snippet->used != 0) {
		str_append_c(snippet, '>');
		snippet_copy(str_c(ctx.quoted_snippet.snippet), snippet);
	}
	pool_unref(&pool);
	return input->stream_errno == 0 ? 0 : -1;
}