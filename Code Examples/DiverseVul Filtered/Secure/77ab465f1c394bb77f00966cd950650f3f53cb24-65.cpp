void js_pushstring(js_State *J, const char *v)
{
	int n = strlen(v);
	CHECKSTACK(1);
	if (n <= soffsetof(js_Value, type)) {
		char *s = STACK[TOP].u.shrstr;
		while (n--) *s++ = *v++;
		*s = 0;
		STACK[TOP].type = JS_TSHRSTR;
	} else {
		STACK[TOP].type = JS_TMEMSTR;
		STACK[TOP].u.memstr = jsV_newmemstring(J, v, n);
	}
	++TOP;
}