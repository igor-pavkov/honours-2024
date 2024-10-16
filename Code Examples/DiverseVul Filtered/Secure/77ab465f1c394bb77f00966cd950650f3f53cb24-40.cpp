static void jsR_callfunction(js_State *J, int n, js_Function *F, js_Environment *scope)
{
	js_Value v;
	int i;

	scope = jsR_newenvironment(J, jsV_newobject(J, JS_COBJECT, NULL), scope);

	jsR_savescope(J, scope);

	if (F->arguments) {
		js_newobject(J);
		if (!J->strict) {
			js_currentfunction(J);
			js_defproperty(J, -2, "callee", JS_DONTENUM);
		}
		js_pushnumber(J, n);
		js_defproperty(J, -2, "length", JS_DONTENUM);
		for (i = 0; i < n; ++i) {
			js_copy(J, i + 1);
			js_setindex(J, -2, i);
		}
		js_initvar(J, "arguments", -1);
		js_pop(J, 1);
	}

	for (i = 0; i < F->numparams; ++i) {
		if (i < n)
			js_initvar(J, F->vartab[i], i + 1);
		else {
			js_pushundefined(J);
			js_initvar(J, F->vartab[i], -1);
			js_pop(J, 1);
		}
	}
	js_pop(J, n);

	jsR_run(J, F);
	v = *stackidx(J, -1);
	TOP = --BOT; /* clear stack */
	js_pushvalue(J, v);

	jsR_restorescope(J);
}