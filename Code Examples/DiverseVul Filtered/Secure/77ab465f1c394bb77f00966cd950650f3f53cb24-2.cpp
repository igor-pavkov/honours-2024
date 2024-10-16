const char *js_tostring(js_State *J, int idx)
{
	return jsV_tostring(J, stackidx(J, idx));
}