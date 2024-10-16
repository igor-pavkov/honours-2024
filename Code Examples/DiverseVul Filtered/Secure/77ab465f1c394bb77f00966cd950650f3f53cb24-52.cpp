int js_toboolean(js_State *J, int idx)
{
	return jsV_toboolean(J, stackidx(J, idx));
}