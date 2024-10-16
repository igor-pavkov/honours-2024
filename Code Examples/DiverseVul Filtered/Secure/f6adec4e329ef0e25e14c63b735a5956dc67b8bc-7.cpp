int option_fetch_parse_recurse_submodules(const struct option *opt,
					  const char *arg, int unset)
{
	int *v;

	if (!opt->value)
		return -1;

	v = opt->value;

	if (unset) {
		*v = RECURSE_SUBMODULES_OFF;
	} else {
		if (arg)
			*v = parse_fetch_recurse_submodules_arg(opt->long_name, arg);
		else
			*v = RECURSE_SUBMODULES_ON;
	}
	return 0;
}