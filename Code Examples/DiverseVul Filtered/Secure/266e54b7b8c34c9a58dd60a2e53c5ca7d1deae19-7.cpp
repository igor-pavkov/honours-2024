int imap_bodystructure_parse_full(const char *bodystructure,
	pool_t pool, struct message_part **parts,
	const char **error_r)
{
	struct istream *input;
	struct imap_parser *parser;
	const struct imap_arg *args;
	char *error = NULL;
	int ret;

	i_assert(*parts == NULL || (*parts)->next == NULL);

	input = i_stream_create_from_data(bodystructure, strlen(bodystructure));
	(void)i_stream_read(input);

	parser = imap_parser_create(input, NULL, SIZE_MAX);
	ret = imap_parser_finish_line(parser, 0,
				      IMAP_PARSE_FLAG_LITERAL_TYPE, &args);
	if (ret < 0) {
		*error_r = t_strdup_printf("IMAP parser failed: %s",
					   imap_parser_get_error(parser, NULL));
	} else if (ret == 0) {
		*error_r = "Empty bodystructure";
		ret = -1;
	} else {
		T_BEGIN {
			ret = imap_bodystructure_parse_args
				(args, pool, parts, error_r);
			if (ret < 0)
				error = i_strdup(*error_r);
		} T_END;

		if (ret < 0) {
			*error_r = t_strdup(error);
			i_free(error);
		}
	}

	imap_parser_unref(&parser);
	i_stream_destroy(&input);
	return ret;
}