static void imap_parser_open_list(struct imap_parser *parser)
{
	parser->list_arg = imap_arg_create(parser);
	parser->list_arg->type = IMAP_ARG_LIST;
	p_array_init(&parser->list_arg->_data.list, parser->pool,
		     LIST_INIT_COUNT);
	parser->cur_list = &parser->list_arg->_data.list;

	parser->cur_type = ARG_PARSE_NONE;
}