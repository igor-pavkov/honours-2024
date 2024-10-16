int js_hasproperty(js_State *J, int idx, const char *name)
{
	return jsR_hasproperty(J, js_toobject(J, idx), name);
}