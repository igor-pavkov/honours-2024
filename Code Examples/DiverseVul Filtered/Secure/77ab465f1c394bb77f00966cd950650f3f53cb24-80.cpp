void js_pushboolean(js_State *J, int v)
{
	CHECKSTACK(1);
	STACK[TOP].type = JS_TBOOLEAN;
	STACK[TOP].u.boolean = !!v;
	++TOP;
}