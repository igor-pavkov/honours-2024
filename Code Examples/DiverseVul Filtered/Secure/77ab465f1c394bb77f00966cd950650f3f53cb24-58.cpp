int js_toint32(js_State *J, int idx)
{
	return jsV_numbertoint32(jsV_tonumber(J, stackidx(J, idx)));
}