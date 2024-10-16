void js_defglobal(js_State *J, const char *name, int atts)
{
	jsR_defproperty(J, J->G, name, atts, stackidx(J, -1), NULL, NULL);
	js_pop(J, 1);
}