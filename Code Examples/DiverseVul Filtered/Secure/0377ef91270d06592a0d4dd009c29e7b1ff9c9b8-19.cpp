static int has_context_option(char *opts)
{
	if (get_option("context=", opts, NULL) ||
	    get_option("fscontext=", opts, NULL) ||
	    get_option("defcontext=", opts, NULL) ||
	    get_option("rootcontext=", opts, NULL))
		return 1;

	return 0;
}