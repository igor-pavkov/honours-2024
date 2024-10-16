void js_pushnull(js_State *J)
{
	CHECKSTACK(1);
	STACK[TOP].type = JS_TNULL;
	++TOP;
}