char *xdg_config_home(const char *filename)
{
	const char *home, *config_home;

	assert(filename);
	config_home = getenv("XDG_CONFIG_HOME");
	if (config_home && *config_home)
		return mkpathdup("%s/git/%s", config_home, filename);

	home = getenv("HOME");
	if (home)
		return mkpathdup("%s/.config/git/%s", home, filename);
	return NULL;
}