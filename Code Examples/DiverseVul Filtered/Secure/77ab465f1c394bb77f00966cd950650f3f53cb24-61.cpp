void js_setglobal(js_State *J, const char *name)
{
	jsR_setproperty(J, J->G, name);
	js_pop(J, 1);
}