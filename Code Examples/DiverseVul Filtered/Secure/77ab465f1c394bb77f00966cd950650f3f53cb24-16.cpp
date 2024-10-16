void js_setproperty(js_State *J, int idx, const char *name)
{
	jsR_setproperty(J, js_toobject(J, idx), name);
	js_pop(J, 1);
}