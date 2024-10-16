static bool imap_parser_read_atom(struct imap_parser *parser,
				  const unsigned char *data, size_t data_size)
{
	size_t i;

	/* read until we've found space, CR or LF. */
	for (i = parser->cur_pos; i < data_size; i++) {
		if (data[i] == ' ' || is_linebreak(data[i])) {
			imap_parser_save_arg(parser, data, i);
			break;
		} else if (data[i] == ')') {
			if (parser->list_arg != NULL ||
			    (parser->flags & IMAP_PARSE_FLAG_INSIDE_LIST) != 0) {
				imap_parser_save_arg(parser, data, i);
				break;
			} else if ((parser->flags &
				    IMAP_PARSE_FLAG_ATOM_ALLCHARS) == 0) {
				parser->error = IMAP_PARSE_ERROR_BAD_SYNTAX;
				parser->error_msg = "Unexpected ')'";
				return FALSE;
			}
			/* assume it's part of the atom */
		} else if (!is_valid_atom_char(parser, data[i]))
			return FALSE;
	}

	parser->cur_pos = i;
	return parser->cur_type == ARG_PARSE_NONE;
}