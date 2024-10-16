void mingw_startup(void)
{
	int i, maxlen, argc;
	char *buffer;
	wchar_t **wenv, **wargv;
	_startupinfo si;

	/* get wide char arguments and environment */
	si.newmode = 0;
	if (__wgetmainargs(&argc, &wargv, &wenv, _CRT_glob, &si) < 0)
		die_startup();

	/* determine size of argv and environ conversion buffer */
	maxlen = wcslen(_wpgmptr);
	for (i = 1; i < argc; i++)
		maxlen = max(maxlen, wcslen(wargv[i]));
	for (i = 0; wenv[i]; i++)
		maxlen = max(maxlen, wcslen(wenv[i]));

	/*
	 * nedmalloc can't free CRT memory, allocate resizable environment
	 * list. Note that xmalloc / xmemdupz etc. call getenv, so we cannot
	 * use it while initializing the environment itself.
	 */
	environ_size = i + 1;
	environ_alloc = alloc_nr(environ_size * sizeof(char*));
	environ = malloc_startup(environ_alloc);

	/* allocate buffer (wchar_t encodes to max 3 UTF-8 bytes) */
	maxlen = 3 * maxlen + 1;
	buffer = malloc_startup(maxlen);

	/* convert command line arguments and environment to UTF-8 */
	__argv[0] = wcstoutfdup_startup(buffer, _wpgmptr, maxlen);
	for (i = 1; i < argc; i++)
		__argv[i] = wcstoutfdup_startup(buffer, wargv[i], maxlen);
	for (i = 0; wenv[i]; i++)
		environ[i] = wcstoutfdup_startup(buffer, wenv[i], maxlen);
	environ[i] = NULL;
	free(buffer);

	/* sort environment for O(log n) getenv / putenv */
	qsort(environ, i, sizeof(char*), compareenv);

	/* fix Windows specific environment settings */
	setup_windows_environment();

	/* initialize critical section for waitpid pinfo_t list */
	InitializeCriticalSection(&pinfo_cs);

	/* set up default file mode and file modes for stdin/out/err */
	_fmode = _O_BINARY;
	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
	_setmode(_fileno(stderr), _O_BINARY);

	/* initialize Unicode console */
	winansi_init();
}