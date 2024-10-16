int imap_parser_read_args(struct imap_parser *parser, unsigned int count,
			  enum imap_parser_flags flags,
			  const struct imap_arg **args_r)
{
	parser->flags = flags;

	if (parser->args_added_extra_eol) {
		/* delete EOL */
		array_pop_back(&parser->root_list);
		parser->args_added_extra_eol = FALSE;
		parser->literal_size_return = FALSE;
	}

	while (!parser->eol && (count == 0 || IS_UNFINISHED(parser) ||
				array_count(&parser->root_list) < count)) {
		if (!imap_parser_read_arg(parser))
			break;

		if (parser->line_size > parser->max_line_size) {
			parser->error = IMAP_PARSE_ERROR_LINE_TOO_LONG;
			parser->error_msg = "IMAP command line too large";
			break;
		}
	}

	if (parser->error != IMAP_PARSE_ERROR_NONE) {
		/* error, abort */
		parser->line_size += parser->cur_pos;
		i_stream_skip(parser->input, parser->cur_pos);
		parser->cur_pos = 0;
		*args_r = NULL;
		return -1;
	} else if ((!IS_UNFINISHED(parser) && count > 0 &&
		    array_count(&parser->root_list) >= count) ||
		   parser->eol || parser->literal_size_return) {
		/* all arguments read / end of line. */
                return finish_line(parser, count, args_r);
	} else {
		/* need more data */
		*args_r = NULL;
		return -2;
	}
}