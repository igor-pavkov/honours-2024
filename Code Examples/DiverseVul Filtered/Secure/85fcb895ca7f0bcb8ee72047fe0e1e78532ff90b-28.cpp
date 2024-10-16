static struct imap_arg *imap_arg_create(struct imap_parser *parser)
{
	struct imap_arg *arg;

	arg = array_append_space(parser->cur_list);
	arg->parent = parser->list_arg;
	return arg;
}