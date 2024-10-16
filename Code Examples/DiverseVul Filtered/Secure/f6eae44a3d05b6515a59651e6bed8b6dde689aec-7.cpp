static char *getusername(uid_t uid)
{
	char *username = NULL;
	struct passwd *password = getpwuid(uid);

	if (password)
		username = password->pw_name;
	return username;
}