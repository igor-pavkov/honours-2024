void js_setregistry(js_State *J, const char *name)
{
	jsR_setproperty(J, J->R, name);
	js_pop(J, 1);
}