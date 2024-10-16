void js_pushvalue(js_State *J, js_Value v)
{
	CHECKSTACK(1);
	STACK[TOP] = v;
	++TOP;
}