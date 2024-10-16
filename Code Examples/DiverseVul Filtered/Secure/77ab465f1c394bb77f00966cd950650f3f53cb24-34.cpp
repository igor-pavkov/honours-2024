void js_getregistry(js_State *J, const char *name)
{
	jsR_getproperty(J, J->R, name);
}