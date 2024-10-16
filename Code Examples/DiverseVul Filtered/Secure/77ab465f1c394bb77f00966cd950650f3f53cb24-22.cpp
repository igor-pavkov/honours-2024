void js_getglobal(js_State *J, const char *name)
{
	jsR_getproperty(J, J->G, name);
}