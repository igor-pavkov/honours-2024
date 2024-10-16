static void jsR_callcfunction(js_State *J, int n, int min, js_CFunction F)
{
	int i;
	js_Value v;

	for (i = n; i < min; ++i)
		js_pushundefined(J);

	F(J);
	v = *stackidx(J, -1);
	TOP = --BOT; /* clear stack */
	js_pushvalue(J, v);
}