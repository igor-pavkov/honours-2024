void js_pushnumber(js_State *J, double v)
{
	CHECKSTACK(1);
	STACK[TOP].type = JS_TNUMBER;
	STACK[TOP].u.number = v;
	++TOP;
}