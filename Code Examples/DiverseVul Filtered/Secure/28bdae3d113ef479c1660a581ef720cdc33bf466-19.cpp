static const char *get_user_name(void)
{
	struct passwd *pw = getpwuid(getuid());
	if (pw != NULL && pw->pw_name != NULL)
		return pw->pw_name;
	else {
		fprintf(stderr, "%s: could not determine username\n", progname);
		return NULL;
	}
}