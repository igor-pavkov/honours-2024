void imap_parser_set_streams(struct imap_parser *parser, struct istream *input,
			     struct ostream *output)
{
	parser->input = input;
	parser->output = output;
}