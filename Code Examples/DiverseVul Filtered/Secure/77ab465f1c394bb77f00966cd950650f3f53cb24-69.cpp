void js_dup(js_State *J)
{
	CHECKSTACK(1);
	STACK[TOP] = STACK[TOP-1];
	++TOP;
}