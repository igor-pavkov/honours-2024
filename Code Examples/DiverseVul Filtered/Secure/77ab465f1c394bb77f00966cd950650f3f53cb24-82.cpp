static void jsR_callscript(js_State *J, int n, js_Function *F, js_Environment *scope)
{
	js_Value v;

	if (scope)
		jsR_savescope(J, scope);

	js_pop(J, n);
	jsR_run(J, F);
	v = *stackidx(J, -1);
	TOP = --BOT; /* clear stack */
	js_pushvalue(J, v);

	if (scope)
		jsR_restorescope(J);
}