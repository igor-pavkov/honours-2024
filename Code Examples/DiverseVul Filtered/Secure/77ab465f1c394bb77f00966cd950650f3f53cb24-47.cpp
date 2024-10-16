void js_getproperty(js_State *J, int idx, const char *name)
{
	jsR_getproperty(J, js_toobject(J, idx), name);
}