void js_pushliteral(js_State *J, const char *v)
{
	CHECKSTACK(1);
	STACK[TOP].type = JS_TLITSTR;
	STACK[TOP].u.litstr = v;
	++TOP;
}