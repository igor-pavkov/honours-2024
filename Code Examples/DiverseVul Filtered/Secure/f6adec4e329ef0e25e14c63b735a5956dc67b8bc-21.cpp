static void warn_multiple_config(const unsigned char *treeish_name,
				 const char *name, const char *option)
{
	const char *commit_string = "WORKTREE";
	if (treeish_name)
		commit_string = sha1_to_hex(treeish_name);
	warning("%s:.gitmodules, multiple configurations found for "
			"'submodule.%s.%s'. Skipping second one!",
			commit_string, name, option);
}