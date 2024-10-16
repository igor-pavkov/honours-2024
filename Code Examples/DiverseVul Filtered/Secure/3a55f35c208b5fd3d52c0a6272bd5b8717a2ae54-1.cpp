static bool snippet_generate(struct snippet_context *ctx,
			     const unsigned char *data, size_t size)
{
	size_t i, count;
	struct snippet_data *target = &ctx->snippet;

	if (ctx->html2text != NULL) {
		buffer_set_used_size(ctx->plain_output, 0);
		mail_html2text_more(ctx->html2text, data, size,
				    ctx->plain_output);
		data = ctx->plain_output->data;
		size = ctx->plain_output->used;
	}

	/* message-decoder should feed us only valid and complete
	   UTF-8 input */

	for (i = 0; i < size; i += count) {
		count = 1;
		switch (ctx->state) {
		case SNIPPET_STATE_NEWLINE:
			if (data[i] == '>') {
				ctx->state = SNIPPET_STATE_QUOTED;
				i++;
				target = &ctx->quoted_snippet;
			} else {
				ctx->state = SNIPPET_STATE_NORMAL;
				target = &ctx->snippet;
			}
			/* fallthrough */
		case SNIPPET_STATE_NORMAL:
		case SNIPPET_STATE_QUOTED:
			snippet_add_content(ctx, target, CONST_PTR_OFFSET(data, i),
					    size-i, &count);
			/* break here if we have enough non-quoted data,
			   quoted data does not need to break here as it's
			   only used if the actual snippet is left empty. */
			if (ctx->snippet.chars_left == 0)
				return FALSE;
			break;
		}
	}
	return TRUE;
}