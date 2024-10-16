static wchar_t *make_environment_block(char **deltaenv)
{
	wchar_t *wenvblk = NULL;
	char **tmpenv;
	int i = 0, size = environ_size, wenvsz = 0, wenvpos = 0;

	while (deltaenv && deltaenv[i])
		i++;

	/* copy the environment, leaving space for changes */
	ALLOC_ARRAY(tmpenv, size + i);
	memcpy(tmpenv, environ, size * sizeof(char*));

	/* merge supplied environment changes into the temporary environment */
	for (i = 0; deltaenv && deltaenv[i]; i++)
		size = do_putenv(tmpenv, deltaenv[i], size, 0);

	/* create environment block from temporary environment */
	for (i = 0; tmpenv[i]; i++) {
		size = 2 * strlen(tmpenv[i]) + 2; /* +2 for final \0 */
		ALLOC_GROW(wenvblk, (wenvpos + size) * sizeof(wchar_t), wenvsz);
		wenvpos += xutftowcs(&wenvblk[wenvpos], tmpenv[i], size) + 1;
	}
	/* add final \0 terminator */
	wenvblk[wenvpos] = 0;
	free(tmpenv);
	return wenvblk;
}