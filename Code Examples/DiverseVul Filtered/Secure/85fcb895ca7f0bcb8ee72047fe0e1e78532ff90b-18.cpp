void imap_parser_read_last_literal(struct imap_parser *parser)
{
	ARRAY_TYPE(imap_arg_list) *list;
	struct imap_arg *last_arg;

	i_assert(parser->literal_size_return);

	last_arg = imap_parser_get_last_literal_size(parser, &list);
	i_assert(last_arg != NULL);

	parser->cur_type = ARG_PARSE_LITERAL_DATA_FORCED;
	i_assert(parser->literal_size == last_arg->_data.literal_size);

	/* delete EOL */
	array_pop_back(&parser->root_list);
	parser->args_added_extra_eol = FALSE;

	/* delete literal size */
	array_pop_back(list);
	parser->literal_size_return = FALSE;
}