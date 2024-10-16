const char *js_nextiterator(js_State *J, int idx)
{
	return jsV_nextiterator(J, js_toobject(J, idx));
}