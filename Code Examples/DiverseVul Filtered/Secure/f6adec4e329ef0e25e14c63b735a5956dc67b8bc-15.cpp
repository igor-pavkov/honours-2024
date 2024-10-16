int parse_fetch_recurse_submodules_arg(const char *opt, const char *arg)
{
	return parse_fetch_recurse(opt, arg, 1);
}