void js_defproperty(js_State *J, int idx, const char *name, int atts)
{
	jsR_defproperty(J, js_toobject(J, idx), name, atts, stackidx(J, -1), NULL, NULL);
	js_pop(J, 1);
}