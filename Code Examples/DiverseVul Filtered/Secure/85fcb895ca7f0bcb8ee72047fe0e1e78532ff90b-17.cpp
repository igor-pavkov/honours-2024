static int finish_line(struct imap_parser *parser, unsigned int count,
		       const struct imap_arg **args_r)
{
	struct imap_arg *arg;
	int ret = array_count(&parser->root_list);

	parser->line_size += parser->cur_pos;
	i_stream_skip(parser->input, parser->cur_pos);
	parser->cur_pos = 0;
	parser->cur_resp_text = FALSE;

	if (parser->list_arg != NULL && !parser->literal_size_return &&
	    (parser->flags & IMAP_PARSE_FLAG_STOP_AT_LIST) == 0) {
		parser->error = IMAP_PARSE_ERROR_BAD_SYNTAX;
		parser->error_msg = "Missing ')'";
		*args_r = NULL;
		return -1;
	}

	arg = array_append_space(&parser->root_list);
	arg->type = IMAP_ARG_EOL;
	parser->args_added_extra_eol = TRUE;

	*args_r = array_get(&parser->root_list, &count);
	return ret;
}