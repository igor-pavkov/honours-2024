static struct managesieve_arg *managesieve_arg_create
(struct managesieve_parser *parser)
{
	struct managesieve_arg *arg;

	arg = array_append_space(parser->cur_list);
	arg->parent = parser->list_arg;

	return arg;
}