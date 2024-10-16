static void js_stackoverflow(js_State *J)
{
	STACK[TOP].type = JS_TLITSTR;
	STACK[TOP].u.litstr = "stack overflow";
	++TOP;
	js_throw(J);
}