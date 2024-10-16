const char *imap_parser_get_error(struct imap_parser *parser,
	enum imap_parser_error *error_r)
{
	if (error_r != NULL)
		*error_r = parser->error;
	return parser->error_msg;
}