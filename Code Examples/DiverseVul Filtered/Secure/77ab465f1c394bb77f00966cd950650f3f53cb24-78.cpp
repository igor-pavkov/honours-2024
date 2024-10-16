void js_pushundefined(js_State *J)
{
	CHECKSTACK(1);
	STACK[TOP].type = JS_TUNDEFINED;
	++TOP;
}