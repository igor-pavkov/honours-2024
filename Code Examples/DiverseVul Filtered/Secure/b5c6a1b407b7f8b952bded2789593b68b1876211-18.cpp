static const char *user_agent(void)
{
	const char *custom = git_libgit2__user_agent();

	if (custom)
		return custom;

	return "libgit2 " LIBGIT2_VERSION;
}