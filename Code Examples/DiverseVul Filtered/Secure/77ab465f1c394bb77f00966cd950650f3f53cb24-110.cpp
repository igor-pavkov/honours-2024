void js_toprimitive(js_State *J, int idx, int hint)
{
	jsV_toprimitive(J, stackidx(J, idx), hint);
}