int mingw_putenv(const char *namevalue)
{
	ALLOC_GROW(environ, (environ_size + 1) * sizeof(char*), environ_alloc);
	environ_size = do_putenv(environ, namevalue, environ_size, 1);
	return 0;
}