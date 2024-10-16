char *js_strdup(js_State *J, const char *s)
{
	int n = strlen(s) + 1;
	char *p = js_malloc(J, n);
	memcpy(p, s, n);
	return p;
}