int imap_parser_finish_line(struct imap_parser *parser, unsigned int count,
			    enum imap_parser_flags flags,
			    const struct imap_arg **args_r)
{
	const unsigned char *data;
	size_t data_size;
	int ret;

	ret = imap_parser_read_args(parser, count, flags, args_r);
	if (ret == -1)
		return -1;
	if (ret == -2) {
		/* we should have noticed end of everything except atom */
		if (parser->cur_type == ARG_PARSE_ATOM) {
			data = i_stream_get_data(parser->input, &data_size);
			imap_parser_save_arg(parser, data, data_size);
		}
	}
	return finish_line(parser, count, args_r);
}