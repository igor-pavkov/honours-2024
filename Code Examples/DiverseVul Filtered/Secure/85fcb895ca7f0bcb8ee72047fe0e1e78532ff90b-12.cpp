bool imap_parser_get_literal_size(struct imap_parser *parser, uoff_t *size_r)
{
	ARRAY_TYPE(imap_arg_list) *list;
	struct imap_arg *last_arg;

	last_arg = imap_parser_get_last_literal_size(parser, &list);
	if (last_arg == NULL)
		return FALSE;

	return imap_arg_get_literal_size(last_arg, size_r);
}