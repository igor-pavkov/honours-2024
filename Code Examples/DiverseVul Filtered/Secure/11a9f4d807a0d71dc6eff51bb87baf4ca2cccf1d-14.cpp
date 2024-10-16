static struct passwd *getpw_str(const char *username, size_t len)
{
	struct passwd *pw;
	char *username_z = xmemdupz(username, len);
	pw = getpwnam(username_z);
	free(username_z);
	return pw;
}