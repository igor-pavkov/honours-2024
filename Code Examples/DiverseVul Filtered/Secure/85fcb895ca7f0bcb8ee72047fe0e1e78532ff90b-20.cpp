static bool imap_parser_literal_end(struct imap_parser *parser)
{
	if (parser->literal_minus && parser->literal_nonsync &&
		parser->literal_size > 4096) {
		parser->error_msg = "Non-synchronizing literal size too large";
		parser->error = IMAP_PARSE_ERROR_LITERAL_TOO_BIG;
		return FALSE;
	}

	if ((parser->flags & IMAP_PARSE_FLAG_LITERAL_SIZE) == 0) {
		if (parser->line_size >= parser->max_line_size ||
		    parser->literal_size >
		    parser->max_line_size - parser->line_size) {
			/* too long string, abort. */
			parser->error = IMAP_PARSE_ERROR_LITERAL_TOO_BIG;
			parser->error_msg = "Literal size too large";
			return FALSE;
		}

		if (parser->output != NULL && !parser->literal_nonsync) {
			o_stream_nsend(parser->output, "+ OK\r\n", 6);
			if (o_stream_is_corked(parser->output)) {
				/* make sure this continuation is sent to the
				   client as soon as possible */
				o_stream_uncork(parser->output);
				o_stream_cork(parser->output);
			}
		}
	}

	parser->cur_type = ARG_PARSE_LITERAL_DATA;
	parser->literal_skip_crlf = TRUE;

	parser->cur_pos = 0;
	return TRUE;
}