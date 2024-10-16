char *mingw_mktemp(char *template)
{
	wchar_t wtemplate[MAX_PATH];
	if (xutftowcs_path(wtemplate, template) < 0)
		return NULL;
	if (!_wmktemp(wtemplate))
		return NULL;
	if (xwcstoutf(template, wtemplate, strlen(template) + 1) < 0)
		return NULL;
	return template;
}