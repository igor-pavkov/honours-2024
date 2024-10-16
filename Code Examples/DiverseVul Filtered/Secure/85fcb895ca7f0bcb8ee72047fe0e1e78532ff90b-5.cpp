void imap_parser_unref(struct imap_parser **_parser)
{
	struct imap_parser *parser = *_parser;

	*_parser = NULL;

	i_assert(parser->refcount > 0);
	if (--parser->refcount > 0)
		return;

	pool_unref(&parser->pool);
	i_free(parser);
}