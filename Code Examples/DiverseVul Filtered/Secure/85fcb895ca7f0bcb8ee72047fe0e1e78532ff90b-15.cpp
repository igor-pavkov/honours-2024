static bool imap_parser_read_literal(struct imap_parser *parser,
				     const unsigned char *data,
				     size_t data_size)
{
	size_t i, prev_size;

	/* expecting digits + "}" */
	for (i = parser->cur_pos; i < data_size; i++) {
		if (data[i] == '}') {
			parser->line_size += i+1;
			i_stream_skip(parser->input, i+1);
			return imap_parser_literal_end(parser);
		}

		if (parser->literal_nonsync) {
			parser->error = IMAP_PARSE_ERROR_BAD_SYNTAX;
			parser->error_msg = "Expecting '}' after '+'";
			return FALSE;
		}

		if (data[i] == '+') {
			parser->literal_nonsync = TRUE;
			continue;
		}

		if (data[i] < '0' || data[i] > '9') {
			parser->error = IMAP_PARSE_ERROR_BAD_SYNTAX;
			parser->error_msg = "Invalid literal size";
			return FALSE;
		}

		prev_size = parser->literal_size;
		parser->literal_size = parser->literal_size*10 + (data[i]-'0');

		if (parser->literal_size < prev_size) {
			/* wrapped around, abort. */
			parser->error = IMAP_PARSE_ERROR_LITERAL_TOO_BIG;
			parser->error_msg = "Literal size too large";
			return FALSE;
		}
	}

	parser->cur_pos = i;
	return FALSE;
}