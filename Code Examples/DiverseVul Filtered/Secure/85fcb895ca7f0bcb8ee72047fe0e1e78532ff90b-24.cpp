static bool is_valid_atom_char(struct imap_parser *parser, char chr)
{
	const char *error_msg;

	if (IS_ATOM_PARSER_INPUT((unsigned char)chr))
		error_msg = "Invalid characters in atom";
	else if ((chr & 0x80) != 0)
		error_msg = "8bit data in atom";
	else
		return TRUE;

	if ((parser->flags & IMAP_PARSE_FLAG_ATOM_ALLCHARS) != 0)
		return TRUE;
	parser->error = IMAP_PARSE_ERROR_BAD_SYNTAX;
	parser->error_msg = error_msg;
	return FALSE;
}