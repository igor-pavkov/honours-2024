static bool imap_parser_close_list(struct imap_parser *parser)
{
	struct imap_arg *arg;

	if (parser->list_arg == NULL) {
		/* we're not inside list */
		if ((parser->flags & IMAP_PARSE_FLAG_INSIDE_LIST) != 0) {
			parser->eol = TRUE;
			parser->cur_type = ARG_PARSE_NONE;
			return TRUE;
		}
		parser->error_msg = "Unexpected ')'";
		parser->error = IMAP_PARSE_ERROR_BAD_SYNTAX;
		return FALSE;
	}

	arg = imap_arg_create(parser);
	arg->type = IMAP_ARG_EOL;

	parser->list_arg = parser->list_arg->parent;
	if (parser->list_arg == NULL) {
		parser->cur_list = &parser->root_list;
	} else {
		parser->cur_list = &parser->list_arg->_data.list;
	}

	parser->cur_type = ARG_PARSE_NONE;
	return TRUE;
}