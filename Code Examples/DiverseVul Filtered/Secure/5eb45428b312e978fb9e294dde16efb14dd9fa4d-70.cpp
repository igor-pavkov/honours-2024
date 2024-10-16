static char *get_username(void)
{
	struct passwd *pwd;

	pwd = getpwuid(getuid());
	if (!pwd) {
		usernic_error("Failed to get username: %s\n", strerror(errno));
		return NULL;
	}

	return pwd->pw_name;
}