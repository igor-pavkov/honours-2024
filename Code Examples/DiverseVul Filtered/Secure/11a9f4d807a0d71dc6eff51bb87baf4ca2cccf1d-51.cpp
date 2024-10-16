char *xdg_cache_home(const char *filename)
{
	const char *home, *cache_home;

	assert(filename);
	cache_home = getenv("XDG_CACHE_HOME");
	if (cache_home && *cache_home)
		return mkpathdup("%s/git/%s", cache_home, filename);

	home = getenv("HOME");
	if (home)
		return mkpathdup("%s/.cache/git/%s", home, filename);
	return NULL;
}