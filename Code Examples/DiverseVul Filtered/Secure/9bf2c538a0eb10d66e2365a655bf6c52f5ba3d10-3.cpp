static int invalid_name(const char *name)
{
	return !*name || strlen(name) > NAME_MAX ||
		strchr(name, '/') || !strcmp(name, ".") || !strcmp(name, "..");
}