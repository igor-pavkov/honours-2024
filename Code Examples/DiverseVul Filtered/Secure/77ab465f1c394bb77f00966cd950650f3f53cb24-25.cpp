static void jsR_getproperty(js_State *J, js_Object *obj, const char *name)
{
	if (!jsR_hasproperty(J, obj, name))
		js_pushundefined(J);
}