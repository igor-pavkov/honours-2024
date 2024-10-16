static bool imap_parser_is_next_resp_text(struct imap_parser *parser)
{
	const struct imap_arg *arg;

	if (parser->cur_list != &parser->root_list ||
	    array_count(parser->cur_list) != 1)
		return FALSE;

	arg = array_front(&parser->root_list);
	if (arg->type != IMAP_ARG_ATOM)
		return FALSE;

	return strcasecmp(arg->_data.str, "OK") == 0 ||
		strcasecmp(arg->_data.str, "NO") == 0 ||
		strcasecmp(arg->_data.str, "BAD") == 0 ||
		strcasecmp(arg->_data.str, "BYE") == 0;
}