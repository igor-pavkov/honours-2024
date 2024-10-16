static bool imap_parser_read_literal_data(struct imap_parser *parser,
					  const unsigned char *data,
					  size_t data_size)
{
	if (parser->literal_skip_crlf) {
		/* skip \r\n or \n, anything else gives an error */
		if (data_size == 0)
			return FALSE;

		if (*data == '\r') {
			parser->line_size++;
			data++; data_size--;
			i_stream_skip(parser->input, 1);

			if (data_size == 0)
				return FALSE;
		}

		if (*data != '\n') {
			parser->error = IMAP_PARSE_ERROR_BAD_SYNTAX;
			parser->error_msg = "Missing LF after literal size";
			return FALSE;
		}

		parser->line_size++;
		data++; data_size--;
		i_stream_skip(parser->input, 1);

		parser->literal_skip_crlf = FALSE;

		i_assert(parser->cur_pos == 0);
	}

	if ((parser->flags & IMAP_PARSE_FLAG_LITERAL_SIZE) == 0 ||
	    parser->cur_type == ARG_PARSE_LITERAL_DATA_FORCED) {
		/* now we just wait until we've read enough data */
		if (data_size < parser->literal_size)
			return FALSE;
		else {
			imap_parser_save_arg(parser, data,
					     (size_t)parser->literal_size);
			parser->cur_pos = (size_t)parser->literal_size;
			return TRUE;
		}
	} else {
		/* we want to save only literal size, not the literal itself. */
		parser->literal_size_return = TRUE;
		imap_parser_save_arg(parser, uchar_empty_ptr, 0);
		return FALSE;
	}
}