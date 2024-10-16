int js_isarrayindex(js_State *J, const char *str, int *idx)
{
	char buf[32];
	*idx = jsV_numbertointeger(jsV_stringtonumber(J, str));
	sprintf(buf, "%u", *idx);
	return !strcmp(buf, str);
}