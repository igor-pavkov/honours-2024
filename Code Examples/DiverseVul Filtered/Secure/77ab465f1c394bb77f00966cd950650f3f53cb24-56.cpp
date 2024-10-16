void js_unref(js_State *J, const char *ref)
{
	js_delregistry(J, ref);
}