void imap_parser_ref(struct imap_parser *parser)
{
	i_assert(parser->refcount > 0);

	parser->refcount++;
}