static int finish_line
(struct managesieve_parser *parser, unsigned int count,
	const struct managesieve_arg **args_r)
{
	struct managesieve_arg *arg;
	int ret = array_count(&parser->root_list);

	parser->line_size += parser->cur_pos;
	i_stream_skip(parser->input, parser->cur_pos);
	parser->cur_pos = 0;

	/* fill the missing parameters with NILs */
	while (count > array_count(&parser->root_list)) {
		arg = array_append_space(&parser->root_list);
		arg->type = MANAGESIEVE_ARG_NONE;
	}
	arg = array_append_space(&parser->root_list);
	arg->type = MANAGESIEVE_ARG_EOL;

	*args_r = array_get(&parser->root_list, &count);
	return ret;
}