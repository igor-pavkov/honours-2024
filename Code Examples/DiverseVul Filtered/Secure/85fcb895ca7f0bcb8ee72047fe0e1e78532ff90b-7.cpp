void imap_parser_reset(struct imap_parser *parser)
{
	p_clear(parser->pool);

	parser->line_size = 0;

	p_array_init(&parser->root_list, parser->pool, LIST_INIT_COUNT);
	parser->cur_list = &parser->root_list;
	parser->list_arg = NULL;

	parser->cur_type = ARG_PARSE_NONE;
	parser->cur_pos = 0;
	parser->cur_resp_text = FALSE;

	parser->str_first_escape = 0;
	parser->literal_size = 0;

	parser->error = IMAP_PARSE_ERROR_NONE;
	parser->error_msg = NULL;

	parser->literal_skip_crlf = FALSE;
	parser->eol = FALSE;
	parser->args_added_extra_eol = FALSE;
	parser->literal_size_return = FALSE;
}