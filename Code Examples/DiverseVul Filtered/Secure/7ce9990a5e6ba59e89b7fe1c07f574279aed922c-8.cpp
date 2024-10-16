int managesieve_parser_read_args
(struct managesieve_parser *parser, unsigned int count,
	enum managesieve_parser_flags flags, const struct managesieve_arg **args_r)
{
	parser->flags = flags;

	while ( !parser->eol && (count == 0 || IS_UNFINISHED(parser)
		|| array_count(&parser->root_list) < count) ) {
		if ( !managesieve_parser_read_arg(parser) )
			break;

		if ( parser->line_size > parser->max_line_size ) {
			parser->error = "MANAGESIEVE command line too large";
			break;
		}
	}

	if ( parser->error != NULL ) {
		/* error, abort */
		parser->line_size += parser->cur_pos;
		i_stream_skip(parser->input, parser->cur_pos);
		parser->cur_pos = 0;
		*args_r = NULL;
		return -1;
	} else if ( (!IS_UNFINISHED(parser) && count > 0
		&& array_count(&parser->root_list) >= count) || parser->eol ) {
		/* all arguments read / end of line. */
		return finish_line(parser, count, args_r);
	} else {
		/* need more data */
		*args_r = NULL;
		return -2;
	}
}