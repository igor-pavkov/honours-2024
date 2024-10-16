js_Value *js_tovalue(js_State *J, int idx)
{
	return stackidx(J, idx);
}