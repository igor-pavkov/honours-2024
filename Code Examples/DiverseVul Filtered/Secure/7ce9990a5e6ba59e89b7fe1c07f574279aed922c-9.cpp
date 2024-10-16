const char *managesieve_parser_get_error
(struct managesieve_parser *parser, bool *fatal)
{
	*fatal = parser->fatal_error;
	return parser->error;
}