static bool imap_parser_is_next_text(struct imap_parser *parser)
{
	const struct imap_arg *arg;
	size_t len;

	if (parser->cur_list != &parser->root_list)
		return FALSE;

	arg = array_back(&parser->root_list);
	if (arg->type != IMAP_ARG_ATOM)
		return FALSE;

	len = strlen(arg->_data.str);
	return len > 0 && arg->_data.str[len-1] == ']';
}