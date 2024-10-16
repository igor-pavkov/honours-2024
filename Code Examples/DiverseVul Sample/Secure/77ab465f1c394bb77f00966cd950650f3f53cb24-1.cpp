static void js_outofmemory(js_State *J)
{
	STACK[TOP].type = JS_TLITSTR;
	STACK[TOP].u.litstr = "out of memory";
	++TOP;
	js_throw(J);
}