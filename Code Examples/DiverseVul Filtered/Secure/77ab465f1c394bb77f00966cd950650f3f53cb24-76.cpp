double js_tonumber(js_State *J, int idx)
{
	return jsV_tonumber(J, stackidx(J, idx));
}