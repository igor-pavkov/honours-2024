void js_delregistry(js_State *J, const char *name)
{
	jsR_delproperty(J, J->R, name);
}