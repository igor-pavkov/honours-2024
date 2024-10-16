int imap_body_parse_from_bodystructure(const char *bodystructure,
				       string_t *dest, const char **error_r)
{
	struct istream *input;
	struct imap_parser *parser;
	const struct imap_arg *args;
	int ret;

	input = i_stream_create_from_data(bodystructure, strlen(bodystructure));
	(void)i_stream_read(input);

	parser = imap_parser_create(input, NULL, SIZE_MAX);
	ret = imap_parser_finish_line(parser, 0, IMAP_PARSE_FLAG_NO_UNESCAPE |
				      IMAP_PARSE_FLAG_LITERAL_TYPE, &args);
	if (ret < 0) {
		*error_r = t_strdup_printf("IMAP parser failed: %s",
					   imap_parser_get_error(parser, NULL));
	} else if (ret == 0) {
		*error_r = "Empty bodystructure";
		ret = -1;
	} else {
		ret = imap_parse_bodystructure_args(args, dest, error_r);
	}

	imap_parser_unref(&parser);
	i_stream_destroy(&input);
	return ret;
}