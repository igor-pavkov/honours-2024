void managesieve_parser_destroy(struct managesieve_parser **parser)
{
	i_stream_unref(&(*parser)->str_stream);

	pool_unref(&(*parser)->pool);
	i_free(*parser);
	*parser = NULL;
}