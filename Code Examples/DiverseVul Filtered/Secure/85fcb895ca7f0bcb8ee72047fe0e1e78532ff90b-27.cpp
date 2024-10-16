static bool imap_parser_read_arg(struct imap_parser *parser)
{
	const unsigned char *data;
	size_t data_size;

	data = i_stream_get_data(parser->input, &data_size);
	if (data_size == 0)
		return FALSE;

	while (parser->cur_type == ARG_PARSE_NONE) {
		/* we haven't started parsing yet */
		if (!imap_parser_skip_to_next(parser, &data, &data_size))
			return FALSE;
		i_assert(parser->cur_pos == 0);

		if (parser->cur_resp_text &&
		    imap_parser_is_next_text(parser)) {
			/* we just parsed [resp-text-code] */
			parser->cur_type = ARG_PARSE_TEXT;
			break;
		}

		switch (data[0]) {
		case '\r':
			if (data_size == 1) {
				/* wait for LF */
				return FALSE;
			}
			if (data[1] != '\n') {
				parser->error = IMAP_PARSE_ERROR_BAD_SYNTAX;
				parser->error_msg = "CR sent without LF";
				return FALSE;
			}
			/* fall through */
		case '\n':
			/* unexpected end of line */
			if ((parser->flags & IMAP_PARSE_FLAG_INSIDE_LIST) != 0) {
				parser->error = IMAP_PARSE_ERROR_BAD_SYNTAX;
				parser->error_msg = "Missing ')'";
				return FALSE;
			}
			parser->eol = TRUE;
			return FALSE;
		case '"':
			parser->cur_type = ARG_PARSE_STRING;
			parser->str_first_escape = -1;
			break;
		case '~':
			/* This could be either literal8 or atom */
			if (data_size == 1) {
				/* wait for the next char */
				return FALSE;
			}
			if (data[1] != '{') {
				parser->cur_type = ARG_PARSE_ATOM;
				break;
			}
			if ((parser->flags & IMAP_PARSE_FLAG_LITERAL8) == 0) {
				parser->error = IMAP_PARSE_ERROR_BAD_SYNTAX;
				parser->error_msg = "literal8 not allowed here";
				return FALSE;
			}
			parser->cur_type = ARG_PARSE_LITERAL8;
			parser->literal_size = 0;
			parser->literal_nonsync = FALSE;
			parser->literal8 = TRUE;
			break;
		case '{':
			parser->cur_type = ARG_PARSE_LITERAL;
			parser->literal_size = 0;
			parser->literal_nonsync = FALSE;
			parser->literal8 = FALSE;
			break;
		case '(':
			imap_parser_open_list(parser);
			if ((parser->flags & IMAP_PARSE_FLAG_STOP_AT_LIST) != 0) {
				i_stream_skip(parser->input, 1);
				return FALSE;
			}
			break;
		case ')':
			if (!imap_parser_close_list(parser))
				return FALSE;

			if (parser->list_arg == NULL) {
				/* end of argument */
				parser->cur_pos++;
				return TRUE;
			}
			break;
		default:
			if (!is_valid_atom_char(parser, data[0]))
				return FALSE;
			parser->cur_type = ARG_PARSE_ATOM;
			break;
		}

		parser->cur_pos++;
	}

	i_assert(data_size > 0);

	switch (parser->cur_type) {
	case ARG_PARSE_ATOM:
		if (!imap_parser_read_atom(parser, data, data_size))
			return FALSE;
		if ((parser->flags & IMAP_PARSE_FLAG_SERVER_TEXT) == 0)
			break;

		if (imap_parser_is_next_resp_text(parser)) {
			/* we just parsed OK/NO/BAD/BYE. after parsing the
			   [resp-text-code] the rest of the message can contain
			   pretty much any random text, which we can't parse
			   as if it was valid IMAP input */
			parser->cur_resp_text = TRUE;
		}
		break;
	case ARG_PARSE_STRING:
		if (!imap_parser_read_string(parser, data, data_size))
			return FALSE;
		break;
	case ARG_PARSE_LITERAL8:
		if (parser->cur_pos == data_size)
			return FALSE;
		if (data[parser->cur_pos] != '{') {
			parser->error = IMAP_PARSE_ERROR_BAD_SYNTAX;
			parser->error_msg = "Expected '{'";
			return FALSE;
		}
		parser->cur_type = ARG_PARSE_LITERAL;
		parser->cur_pos++;
		/* fall through */
	case ARG_PARSE_LITERAL:
		if (!imap_parser_read_literal(parser, data, data_size))
			return FALSE;

		/* pass through to parsing data. since input->skip was
		   modified, we need to get the data start position again. */
		data = i_stream_get_data(parser->input, &data_size);

		/* fall through */
	case ARG_PARSE_LITERAL_DATA:
	case ARG_PARSE_LITERAL_DATA_FORCED:
		if (!imap_parser_read_literal_data(parser, data, data_size))
			return FALSE;
		break;
	case ARG_PARSE_TEXT:
		if (!imap_parser_read_text(parser, data, data_size))
			return FALSE;
		break;
	default:
                i_unreached();
	}

	i_assert(parser->cur_type == ARG_PARSE_NONE);
	return TRUE;
}