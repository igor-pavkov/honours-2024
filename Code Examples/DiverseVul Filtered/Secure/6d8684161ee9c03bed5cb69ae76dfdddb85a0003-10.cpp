char *mingw_getenv(const char *name)
{
	char *value;
	int pos = bsearchenv(environ, name, environ_size - 1);
	if (pos < 0)
		return NULL;
	value = strchr(environ[pos], '=');
	return value ? &value[1] : NULL;
}