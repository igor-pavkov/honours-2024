static void PrintHelp(char *arg_name, Bool search_desc, Bool no_match)
{
	GF_FilterSession *fs;
	Bool res;

	fs = gf_fs_new_defaults(0);

	if (arg_name[0]=='-')
		arg_name++;

	if (search_desc) {
		char *_arg_name = gf_strdup(arg_name);
		strlwr(_arg_name);
		GF_LOG(GF_LOG_INFO, GF_LOG_APP, ("Possible options mentionning `%s`:\n", arg_name));
		PrintHelpArg(_arg_name, SEARCH_DESC, fs);
		gf_free(_arg_name);
	} else {
		res = no_match ? GF_FALSE : PrintHelpArg(arg_name, SEARCH_ARG_EXACT, fs);
		if (!res) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_APP, ("Option -%s unknown, please check usage.\n", arg_name));
			GF_LOG(GF_LOG_INFO, GF_LOG_APP, ("Possible options are:\n"));

			PrintHelpArg(arg_name, SEARCH_ARG_CLOSE, fs);
		}
	}
	if (fs)
		gf_fs_del(fs);
}